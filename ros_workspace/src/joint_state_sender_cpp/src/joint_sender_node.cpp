#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <string>
#include <vector>
#include <cmath>      // For M_PI, std::round, std::abs
#include <sstream>    // For building formatted output strings
#include <functional> // For std::bind and std::placeholders

// POSIX headers for serial communication on Linux
#include <fcntl.h>   // File control options (O_RDWR, etc.)
#include <termios.h> // POSIX terminal control definitions
#include <unistd.h>  // For read(), write(), close()

// Serial port configuration
const std::string SERIAL_PORT = "/dev/ttyACM0";
const speed_t BAUD_RATE = B115200;

/**
 * @class JointSenderNode
 * @brief A ROS 2 node that subscribes to /joint_states and sends joint positions (in degrees)
 *        to a serial device via UART.
 */
class JointSenderNode : public rclcpp::Node
{
public:
    JointSenderNode() : Node("joint_sender_node")
    {
        // Initialize serial communication
        serial_port_fd_ = this->init_serial();

        if (serial_port_fd_ < 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to initialize serial port. Shutting down node.");
            rclcpp::shutdown();
            return;
        }

        // Create a subscriber to the /joint_states topic
        subscription_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states", 10,
            std::bind(&JointSenderNode::joint_state_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Node started. Listening to /joint_states and sending to %s", SERIAL_PORT.c_str());
    }

    ~JointSenderNode()
    {
        // Close serial port on shutdown
        if (serial_port_fd_ >= 0)
        {
            RCLCPP_INFO(this->get_logger(), "Closing serial port.");
            close(serial_port_fd_);
        }
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscription_;
    int serial_port_fd_;                 // File descriptor for the serial port
    std::vector<int> last_sent_degrees_; // Stores the last sent joint positions in degrees

    /**
     * @brief Initializes and configures the serial port.
     * @return File descriptor on success, -1 on failure.
     */
    int init_serial()
    {
        int fd = open(SERIAL_PORT.c_str(), O_RDWR | O_NOCTTY);
        if (fd < 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Cannot open port %s. Error: %s", SERIAL_PORT.c_str(), strerror(errno));
            RCLCPP_WARN(this->get_logger(), "Check permissions: sudo chmod 666 %s", SERIAL_PORT.c_str());
            return -1;
        }

        struct termios tty;
        if (tcgetattr(fd, &tty) != 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to get terminal attributes.");
            close(fd);
            return -1;
        }

        // Configure baud rate
        cfsetospeed(&tty, BAUD_RATE);
        cfsetispeed(&tty, BAUD_RATE);

        // Configure serial communication: 8 data bits, no parity, 1 stop bit (8N1)
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~CRTSCTS;       // Disable hardware flow control
        tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

        // Disable canonical mode and echo
        tty.c_lflag &= ~ICANON;
        tty.c_lflag &= ~ECHO;
        tty.c_lflag &= ~ECHOE;
        tty.c_lflag &= ~ECHONL;
        tty.c_lflag &= ~ISIG;

        // Configure input mode: disable software flow control and special character processing
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

        // Configure output mode: raw output
        tty.c_oflag &= ~OPOST;
        tty.c_oflag &= ~ONLCR;

        // Non-blocking mode (no read timeout)
        tty.c_cc[VTIME] = 0;
        tty.c_cc[VMIN] = 0;

        // Apply configuration
        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
            RCLCPP_ERROR(this->get_logger(), "Failed to set terminal attributes.");
            close(fd);
            return -1;
        }

        RCLCPP_INFO(this->get_logger(), "Serial port %s successfully opened and configured.", SERIAL_PORT.c_str());
        return fd;
    }

    /**
     * @brief Callback function executed when a new JointState message is received.
     * @param msg The received JointState message.
     */
    void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
    {
        // Convert joint positions from radians to integer degrees
        std::vector<int> current_degrees;
        for (const double &rad_position : msg->position)
        {
            double degrees = rad_position * (180.0 / M_PI);
            current_degrees.push_back(static_cast<int>(std::round(degrees)));
        }

        bool should_send = false;

        // Determine if data should be sent
        if (last_sent_degrees_.empty() || last_sent_degrees_.size() != current_degrees.size())
        {
            should_send = true;
            if (last_sent_degrees_.empty())
            {
                RCLCPP_INFO(this->get_logger(), "First message received. Sending initial state.");
            }
            else
            {
                RCLCPP_WARN(this->get_logger(), "Number of joints has changed. Sending updated state.");
            }
        }
        else
        {
            // Check if any joint changed by at least 1 degree
            for (size_t i = 0; i < current_degrees.size(); ++i)
            {
                if (std::abs(current_degrees[i] - last_sent_degrees_[i]) >= 1)
                {
                    should_send = true;
                    break;
                }
            }
        }

        // Skip transmission if no significant change detected
        if (!should_send)
        {
            return;
        }

        // Update the last sent state
        last_sent_degrees_ = current_degrees;

        // Format output string (e.g., "30 0 45 -15 0 0\n")
        std::stringstream ss;
        for (size_t i = 0; i < current_degrees.size(); ++i)
        {
            ss << current_degrees[i];
            if (i < current_degrees.size() - 1)
            {
                ss << " ";
            }
        }
        ss << "\n";

        std::string output_string = ss.str();

        // Send formatted joint data over UART
        ssize_t bytes_written = write(serial_port_fd_, output_string.c_str(), output_string.length());

        if (bytes_written < 0)
        {
            RCLCPP_WARN(this->get_logger(), "Serial write error.");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Sent via UART: %s", output_string.c_str());
        }
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<JointSenderNode>();

    // Spin only if initialization succeeded
    if (rclcpp::ok())
    {
        rclcpp::spin(node);
    }

    rclcpp::shutdown();
    return 0;
}

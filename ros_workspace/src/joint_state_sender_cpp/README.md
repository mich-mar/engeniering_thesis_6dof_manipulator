

# 🦾 joint_sender_node

**A ROS 2 package for sending robot joint states over a serial (UART) connection.**

---

## 🧩 Overview

The **`joint_sender_node`** package provides a ROS 2 node that subscribes to the standard `/joint_states` topic and transmits joint position data (converted from radians to degrees) through a serial port to an external conrtoler **ESP32**.

This enables synchronization between **ROS2 simulated robot** and **physical actuators** for real-time hardware control.

---

## 🚀 Features

* Subscribes to `/joint_states` (`sensor_msgs/msg/JointState`).
* Converts joint positions from **radians** to **integer degrees**.
* Sends joint angles as a formatted text string over **UART**.
* Only transmits when a joint angle changes by **≥ 1 degree**.
* Configures serial port automatically (8N1, 115200 baud).
* Provides detailed **ROS 2 logging** for debugging and monitoring.
* Clean, modern C++ (ROS 2 `rclcpp`) implementation.

---

## 🛠️ Installation

### 1. Clone the package into your ROS 2 workspace

### 2. Build the package

```bash
cd ~/ros2_ws
colcon build --packages-select joint_sender_node
```

### 3. Source the workspace

```bash
source install/setup.bash
```

---

## ⚙️ Usage

Run the node manually after launching a robot simulation or joint publisher:

```bash
ros2 run joint_state_sender_cpp joint_sender_node
```

### Parameters

| Parameter     | Type   | Default        | Description                 |
| ------------- | ------ | -------------- | --------------------------- |
| `serial_port` | string | `/dev/ttyACM0` | Serial device path          |
| `baud_rate`   | int    | `115200`       | Baud rate for communication |

---

## 🧾 Message Format

Each transmission contains all joint angles in degrees, separated by spaces, followed by a newline character:

```
30 0 45 -15 0 0 \n
```

---

## 🧰 Dependencies

**Runtime:**

* `rclcpp`
* `sensor_msgs`

**System:**

* Linux with POSIX serial API
* Read/write access to serial port (`/dev/ttyACM0`)

If you encounter permission errors:

```bash
sudo chmod 666 /dev/ttyACM0
```

---

## 🧪 Example Integration

1. Run a robot simulation in MoveIt package (manipulator_moveit_package) that publishes `/joint_states`.
2. Connect an ESP32 over USB (appears as `/dev/ttyACM0`).
3. Start the node:

   ```bash
   ros2 run joint_state_sender_cpp joint_sender_node
   ```
4. Observe transmitted joint data in real robot movement.

---

## 🩺 Debugging Tips

* If the serial port fails to open, check device permissions:

  ```bash
  ls -l /dev/ttyACM*
  ```
* To monitor raw data:

  ```bash
  screen /dev/ttyACM0 115200
  ```
* Use ROS 2 comand to verify incoming data.

  ```bash
  ros2 topic echo /joint_states
  ```

---

## 📁 File Structure

```
joint_sender/
├── CMakeLists.txt
├── package.xml
├── src/
│   └── joint_sender_node.cpp
└── README.md
```

---

## 🧾 License

This package is released under the **MIT License**.
See the [LICENSE](LICENSE) file for details.

---

## 👤 Author

**Michał Markuzel**
📧 [markuzel.michal@gmail.com](mailto:markuzel.michal@gmail.com)



# 🤖 ROS 2 Manipulator Workspace

**A complete ROS 2 workspace for simulating, visualizing, and communicating with a custom robotic manipulator.**

---

## 🧩 Overview

This workspace provides a modular environment for both **simulation and real hardware control** of a robotic manipulator.
It combines:

* **URDF and visualization** in RViz & Gazebo
* **Motion planning** via MoveIt 2
* **Serial communication** with external controller (ESP32)

The design enables a seamless workflow between simulation and physical actuation.

---

## 🧭 System Architecture

Below is the high-level data flow between packages:

<p align="center">
  <img src="docs/diagram.svg" width="700" alt="ROS 2 Manipulator Architecture Diagram">
</p>


---

## 📁 Workspace Structure

```
ros_workspace/
├── src/
│   ├── manipulator_ros2_description/   # URDF/Xacro model, RViz, Gazebo setup
│   ├── manipulator_moveit_package/     # MoveIt 2 configuration and controllers
│   ├── joint_state_sender_cpp/         # UART node for external hardware
│   ├── moveit2/, moveit_resources/, moveit_visual_tools/ ...
│   └── other dependencies (serial, rviz_visual_tools, etc.)
```

---

## ⚙️ Setup Instructions

### 1️⃣ Build the workspace

```bash
cd ~/ros_workspace
colcon build
```

### 2️⃣ Source the workspace

```bash
source install/setup.bash
```
---

## 🚀 Running the System

### 🟢 1. Visualize the Manipulator

Visualize your robot model and control joint sliders in **RViz**.

```bash
ros2 launch manipulator_ros2_description display.launch.py
```

**Features:**

* Displays the full robot model (URDF/Xacro).
* GUI sliders for manual joint control.
* Useful for testing the robot description and transformations.

---

### ⚙️ 2. Run MoveIt 2 Simulation

Launches the **MoveIt 2 environment** with motion planning and controller simulation.

```bash
ros2 launch manipulator_moveit_package demo_with_controlers.launch.py
```

**Features:**

* Realistic motion planning using OMPL or Pilz.
* Controllers simulate actuator motion.
* Publishes `/joint_states` for visualization or hardware streaming.

---

### 🧠 3. Transmit Joint Data via UART

Starts the **C++ joint sender node**, which subscribes to `/joint_states` and sends joint angles to a serial device.

```bash
ros2 run joint_state_sender_cpp joint_sender_node
```

**Features:**

* Converts radians → degrees.
* Sends formatted data to `/dev/ttyACM0` (default 115200 baud).
* Ideal for testing on real manipulators with microcontrollers.

> Ensure correct permissions for serial port:
>
> ```bash
> sudo chmod 666 /dev/ttyACM0
> ```

---

## 🔄 Typical Workflow

1. **Start MoveIt simulation (Terminal 1):**

   ```bash
   ros2 launch manipulator_moveit_package demo_with_controlers.launch.py
   ```
2. **Start UART sender (Terminal 2):**

   ```bash
   ros2 run joint_state_sender_cpp joint_sender_node
   ```
3. **Visualize or monitor robot motion:**

   * Use RViz to observe the manipulator.
   * External hardware receives live joint data.

---

## 🧰 Dependencies

Ensure you have these installed (replace `${ROS_DISTRO}` with your ROS 2 version):

```bash
sudo apt install ros-${ROS_DISTRO}-moveit \
                 ros-${ROS_DISTRO}-gazebo-ros \
                 ros-${ROS_DISTRO}-joint-state-publisher-gui \
                 ros-${ROS_DISTRO}-robot-state-publisher \
                 ros-${ROS_DISTRO}-xacro \
                 ros-${ROS_DISTRO}-ros2-control
```

---

## 🧩 Package Summary

| Package                          | Purpose                                               |
| -------------------------------- | ----------------------------------------------------- |
| **manipulator_ros2_description** | Robot model, meshes, RViz & Gazebo visualization      |
| **manipulator_moveit_package**   | Motion planning, kinematics, and control via MoveIt 2 |
| **joint_state_sender_cpp**       | Custom C++ UART node for hardware communication       |

---

## 📜 License

Distributed under the **MIT License**.
See individual package directories for license details.

---

## 👤 Author

**Michał Markuzel**
📧 [markuzel.michal@gmail.com](mailto:markuzel.michal@gmail.com)

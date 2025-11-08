
# 🦾 manipulator_moveit_package

**A ROS 2 MoveIt configuration package for simulating and visualizing a custom manipulator.**

---

## 🧩 Overview

The **`manipulator_moveit_package`** contains the **MoveIt 2** configuration for a custom robotic manipulator.
It was automatically generated using the **MoveIt Setup Assistant** and provides a ready-to-use motion planning environment in simulation.

In this project, the package is primarily used to:

* Run a **MoveIt simulation** with controllers.
* Publish **joint state data** to the `/joint_states` topic.

---

## ⚙️ Primary Launch File

### `demo_with_controlers.launch.py`

This launch file starts the **MoveIt demo simulation** with ROS 2 controllers enabled.
It provides a complete simulated robot environment, including:

* `move_group` for motion planning.
* `joint_state_publisher` and `robot_state_publisher`.
* Active controllers defined in `ros2_controllers.yaml`.
* RViz visualization (`moveit.rviz`).

**Run the simulation:**

```bash
ros2 launch manipulator_moveit_package demo_with_controlers.launch.py
```

Once launched, the simulation:

* Publishes joint positions to `/joint_states`.
* Allows interactive motion planning through RViz.
* Simulates controller feedback for each joint.

This topic can be read by other nodes such as:

```bash
ros2 run joint_state_sender_cpp joint_sender_node
```

to transmit real-time joint positions to an external device over a serial connection.

---

## 📁 Package Structure

```
manipulator_moveit_package/
├── config/
│   ├── *.yaml                        # Motion planning, kinematics, and controller configs
│   ├── manipulator_ros2.srdf         # Semantic robot description
│   ├── manipulator_ros2.urdf.xacro   # Robot model used by MoveIt
│   ├── manipulator_ros2.ros2_control.xacro
│   └── moveit.rviz                   # Preconfigured RViz layout
├── launch/
│   ├── demo_with_controlers.launch.py  # Main simulation launch file
│   ├── demo.launch.py
│   ├── move_group.launch.py
│   ├── moveit_rviz.launch.py
│   └── other MoveIt setup launch files
├── CMakeLists.txt
└── package.xml
```

---

## 🚀 Usage

1. **Source your workspace:**

   ```bash
   source ~/ros_workspace/install/setup.bash
   ```

2. **Start the MoveIt demo with controllers:**

   ```bash
   ros2 launch manipulator_moveit_package demo_with_controlers.launch.py
   ```

3. **Visualize in RViz:**

   * The manipulator model loads automatically.
   * You can use the **Motion Planning** panel to move the robot.
   * The simulation continuously publishes joint positions on `/joint_states`.

4. **Integrate with the UART sender node:**

   ```bash
   ros2 run joint_state_sender_cpp joint_sender_node
   ```

   The node subscribes to `/joint_states` and transmits current joint angles to your external hardware via the serial port.

---

## 🧠 How It Fits Into the System

**Overall data flow:**

```
MoveIt Simulation → /joint_states → joint_sender_node → UART → ESP32
```

This setup allows you to test motion planning and hardware communication in a complete virtual environment.

---

## 🧰 Configuration Files

The `config/` directory includes:

* **`ompl_planning.yaml`** – Default motion planning configuration.
* **`kinematics.yaml`** – Inverse kinematics solver settings.
* **`ros2_controllers.yaml`** – Controller definitions for simulation.
* **`joint_limits.yaml`** – Velocity and position limits for each joint.
* **`manipulator_ros2.urdf.xacro`** – The robot’s URDF used by MoveIt.
* **`manipulator_ros2.srdf`** – Semantic robot model (group, joint, and link definitions).
* **`moveit.rviz`** – Default RViz visualization setup.

---

## 🧾 Dependencies

Required runtime dependencies:

* `moveit_ros_planning_interface`
* `moveit_ros_move_group`
* `moveit_visual_tools`
* `robot_state_publisher`
* `ros2_control`
* `gazebo_ros`

Install them (if missing):

```bash
sudo apt install ros-${ROS_DISTRO}-moveit \
                 ros-${ROS_DISTRO}-ros2-control \
                 ros-${ROS_DISTRO}-gazebo-ros \
                 ros-${ROS_DISTRO}-joint-state-publisher-gui
```

---

## 🧩 Typical Workflow

1. Launch `demo_with_controlers.launch.py` to start the MoveIt simulation.
2. Run `joint_state_sender_cpp` to read these positions and forward them via UART.
3. Use RViz to plan and execute trajectories interactively.

---

## 📜 License

Released under the **MIT License**.
See the [LICENSE](LICENSE) file for details.

---

## 👤 Author

**Michał Markuzel**
📧 [markuzel.michal@gmail.com](mailto:markuzel.michal@gmail.com)

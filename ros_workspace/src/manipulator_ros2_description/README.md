

# 🤖 manipulator_ros2_description

**A ROS 2 description package containing the 3D model, URDF/Xacro configuration, and simulation launch files for a custom robotic manipulator.**

---

## 🧩 Overview

The **`manipulator_ros2_description`** package defines the kinematic and visual model of a custom 6-DOF robotic manipulator.
It was automatically generated from **Autodesk Fusion 360** and adapted for **ROS 2** integration.

The package provides:

* Complete **URDF/Xacro** robot description.
* **Gazebo** simulation setup for testing in a physics environment.
* **RViz** visualization configuration for motion and joint-state testing.
* STL mesh geometry for all robot links and the end effector.

This package is typically used together with controller or hardware-interface nodes to visualize and simulate the manipulator in ROS 2.

---

## 📁 Package Structure

```
manipulator_ros2_description/
├── config/
│   └── display.rviz              # Predefined RViz visualization settings
├── launch/
│   ├── display.launch.py         # Launch file for RViz visualization
│   └── gazebo.launch.py          # Launch file for Gazebo simulation
├── meshes/                       # STL geometry of individual links
│   ├── base_link.stl
│   ├── efector_1.stl
│   ├── link_1_1.stl
│   ├── link_2_1.stl
│   ├── link_3_1.stl
│   ├── link_4_1.stl
│   └── link_5_1.stl
├── urdf/
│   ├── manipulator_ros2.xacro    # Main robot description (URDF/Xacro)
│   ├── manipulator_ros2.gazebo   # Gazebo-specific configuration (materials, plugins)
│   ├── manipulator_ros2.trans    # Transformation / transmission definitions
│   └── materials.xacro           # Material and color definitions
├── resource/                     # Required by ROS 2 for package indexing
├── package.xml
├── setup.py / setup.cfg          # Python package metadata (for ament)
└── test/                         # Basic style and license compliance tests
```

---

## 🦾 Features

* Full **kinematic chain** and link geometry from Fusion 360.
* Predefined **materials**, **colors**, and **transmissions**.
* **Gazebo launch file** for dynamic simulation with gravity and collisions.
* **RViz launch file** for visualization and manual joint control.
* Compatible with standard ROS 2 tools (`joint_state_publisher`, `robot_state_publisher`, `gazebo_ros`).
* Ready for integration with control packages (e.g., `ros2_control`, `joint_sender`, etc.).

---

## 🚀 Launch Instructions

### 🟢 RViz Visualization

Displays the manipulator model in RViz and allows manual joint control through sliders.

```bash
ros2 launch manipulator_ros2_description display.launch.py
```

This launch file loads:

* `manipulator_ros2.xacro`
* `joint_state_publisher_gui`
* `robot_state_publisher`
* RViz with the preconfigured layout from `config/display.rviz`

### ⚙️ Gazebo Simulation

Launches the robot in a Gazebo world with physics enabled.

```bash
ros2 launch manipulator_ros2_description gazebo.launch.py
```

This file loads:

* The robot description (`URDF/Xacro`)
* Gazebo plugins and controllers defined in `manipulator_ros2.gazebo`
* Optional ROS 2 control interfaces (if configured)

---

## 🧠 Typical Workflow

1. **Visualize in RViz** to verify model correctness and joint hierarchy.
2. **Simulate in Gazebo** to test physical motion, limits, and collisions.
3. **Integrate with control packages** (e.g., `joint_sender`, `ros2_control`) for real-time actuation.

---

## 🧾 Dependencies

Runtime dependencies (declared in `package.xml`):

* `rclpy`
* `xacro`
* `robot_state_publisher`
* `joint_state_publisher_gui`
* `gazebo_ros`
* `urdf`

---

## 🧩 Notes

* The geometry files (`.stl`) were exported directly from **Fusion 360**.
* Joints, links, and transforms follow the same naming convention as in the original CAD model.
* The `materials.xacro` file defines visual properties for RViz and Gazebo rendering.

---

## 📜 License

Released under the **MIT License**.
See the [LICENSE](LICENSE) file for details.

---

## 👤 Author

**Michał Markuzel**
📧 [markuzel.michal@gmail.com](mailto:markuzel.michal@gmail.com)



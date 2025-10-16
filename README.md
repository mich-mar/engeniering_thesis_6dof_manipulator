# 6DOF Industrial Robotic Manipulator

This engineering project focuses on designing and building a **six-degree-of-freedom (6DOF) industrial robotic manipulator** at a reduced scale. The manipulator is first designed in a **CAD environment** and then manufactured using **3D printing technology**. The goal of the project is to create a fully functional model that allows for the study of basic robotic kinematics and control principles.

In addition to the physical model, a **digital twin** of the manipulator is developed in **ROS 2 (Robot Operating System 2)**. The simulation enables control of individual joints and Cartesian movement of the end-effector along the X, Y, and Z axes. Simulation tools such as **Gazebo**, **RViz**, **URDF/Xacro**, and **MoveIt 2** are used to visualize and test the robot’s behavior in a virtual environment.

The research aspect of the project includes studying literature on robotic arm modeling, developing the mathematical description of forward and inverse kinematics, and implementing control algorithms in ROS 2.  
The engineering aspect covers CAD design, development of the mechanical and electronic components, and implementation of embedded control software.

This repository contains:
- 3D models of the manipulator,
- electronic schematics,
- firmware for the microcontroller (ESP32 S3),
- ROS 2 project configuration (URDF, Xacro, launch files, simulation packages, and control scripts).

The project is conducted as an **engineering thesis** under the supervision of **Dr. Eng. Michał Błędowski**.

### References
- [ROS 2 Documentation (Foxy)](https://docs.ros.org/en/foxy/index.html)  
- [Inverse Kinematics – MathWorks](https://www.mathworks.com/discovery/inverse-kinematics.html)  
- [Computer-Aided Design – Wikipedia](https://en.wikipedia.org/wiki/Computer-aided_design)


## 📘 6DOF Manipulator Controller

### 🎯 Project Overview

This project implements a **modular control system** for a 6-DOF robotic manipulator using an **ESP32 microcontroller** and **standard servo motors**.
The system supports both **feedback-controlled joints** (hybrid servos) and **open-loop standard servos**, allowing precise position control and flexible configuration.

The project is designed for integration with a **serial command interface**, enabling external control (e.g., via ROS 2 packages).

---

### 🧩 Project Structure

```
manipulator_controller/
├── include/                  # Header files shared between modules
├── lib/                      # External and user-defined libraries
├── src/                      # Main application source files
│   ├── config/               # Configuration and constants
│   │   ├── Constants.h       # Pin definitions, calibration values, limits
│   │   └── ServoPositions.h  # Predefined servo angle sets (default, pos1, etc.)
│   ├── control/              # Servo control logic
│   │   ├── ServoControl.cpp  # Implementation of hybrid and standard servo control
│   │   └── ServoControl.h
│   ├── io/                   # Input/output communication
│   │   ├── SerialCommands.cpp # Command parsing and serial interface handling
│   │   └── SerialCommands.h
│   └── main.cpp              # Application entry point and main loop
├── test/                     # Unit/integration tests (optional)
└── platformio.ini            # PlatformIO configuration (build environment)
```

---

### ⚙️ Features

* Hybrid feedback control for joint 0 using analog voltage feedback
* Smooth incremental movement for standard servos (joints 1–5)
* Predefined pose commands (`default`, `pos1`, `pos2`, `pos3`, `zero_pos`)
* Custom offset command mode (`30 0 -10 5 0 0`)
* Modular C++ architecture with separate layers for:

  * **Configuration**
  * **Control logic**
  * **Serial I/O**
  * **Utility functions (helpers)**

---

### 🧠 Modular Design Overview

| Module                | Description                                                          |
| --------------------- | -------------------------------------------------------------------- |
| `config/`             | Stores calibration constants and reference positions for servos      |
| `control/`            | Implements motion logic and hybrid feedback control loop             |
| `io/`                 | Handles command input over serial interface                          |
| `utils/` *(optional)* | Provides reusable helper functions (math, filtering, clamping, etc.) |

---

### 🔧 Dependencies

* **PlatformIO** (ESP32 environment)
* **ESP32Servo** library
* **Arduino Core for ESP32**

---

### 💬 Serial Commands

You can control the manipulator via a serial terminal (e.g., PlatformIO monitor or PuTTY):

| Command                  | Description                                       |
| ------------------------ | ------------------------------------------------- |
| `default`                | Move to default servo positions                   |
| `pos1` / `pos2` / `pos3` | Move to predefined poses                          |
| `zero_pos`               | Move to zero calibration pose                     |
| `<offsets>`              | Move by relative offsets (e.g., `30 0 -10 5 0 0`) |

---

### 🧩 Example Communication

```
---- Sent utf8 encoded message: "zero_pos\r\n" ----
COMMAND: Setting 'zero_pos' position.
NEW TARGETS: 0.00 130.00 130.00 80.00 80.00 90.00 

---- Sent utf8 encoded message: "pos1\r\n" ----
COMMAND: Setting 'pos1' position.
NEW TARGETS: 0.00 120.00 150.00 90.00 90.00 180.00 

---- Sent utf8 encoded message: "30 10 -20 -50 90 -90 \r\n" ----
OFFSETS: 30.0 10.0 -20.0 -50.0 90.0 -90.0 
NEW TARGETS: -30.00 140.00 150.00 30.00 170.00 0.00 
```

---

## 👤 Author

**Michał Markuzel**
📧 [markuzel.michal@gmail.com](mailto:markuzel.michal@gmail.com)
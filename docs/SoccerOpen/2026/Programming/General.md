# General

## Abstract
We structured our codebase into **modular libraries** and used **GitHub** to support collaborative development. Each library corresponds to a specific hardware component, which improves code readability and contributes to a more **maintainable and scalable system**. This modular approach has also allowed us to debug efficiently and make quick adjustments when needed.

For our workflow we used **Visual Studio Code** as the IDE along **PlatformIO** extension, which allowed us to easily work in libraries, src, testcodes and environment changes. We worked using a **Teensy 4.1** as the main microcontroller. For OpenMV Camera programming we used the **official OpenMV IDE**.

GitHub serves as our central platform for **version control, file sharing, and team coordination**. It hosts all our software and hardware files, making them easily accessible to every team member regardless of time or location. This ensures everyone is always working with the most up-to-date versions and helps maintain smooth collaboration across the team. It also helped mentors check our updates on software development for the contest.

## PlatformIO
The firmware development for this project was carried out using **PlatformIO**, an open-source ecosystem for embedded systems development integrated as an extension within Visual Studio Code. PlatformIO centralizes **toolchain management, library dependencies, build systems, and firmware flashing** into a single unified environment, eliminating the need to configure each tool separately.

Project configuration is handled through a **`platformio.ini` file**, which defines the target board, framework, upload protocol, and library dependencies in a declarative format. This approach simplifies onboarding for new team members and ensures a **consistent development environment** across different machines.

For starters it is recommended to look at the [PlatformIO documentation](https://docs.platformio.org/en/latest/integration/ide/pioide.html) to correctly install the tool, provided that it is very useful and intuitive to work with.

## General Recommendations / Workflow

There are some general recommendations for programmers on how to begin working on the project. If you know very little to nothing about programming itself or its IDEs, it is highly recommended to first cover the basics on the following topics:

  **Languages: Python and C++**
  For working with the robot and the development of the code, first know all the basics about **C++ and Python**. It is very likely that you will be working with both. Learn all you can about **C++ syntax**, then **conditionals and loops**, and lastly how **classes and objects** work. Learn the same fundamentals in Python, since it is the language used for **vision development**.

  **IDEs**
  Learn all you can about the correct use of the IDEs chosen to work on the project. In this case the project was developed using **VS Code, PlatformIO, and OpenMV IDE** as previously mentioned. Learn the basics for all of these before getting into development itself.

  **Git / GitHub**
  It is highly recommended to get completely comfortable with **Git and GitHub**. At minimum, know how **repositories** work and how to **commit and merge** correctly.

## Algorithm Structure

The codebase is organized into **modular libraries**, each corresponding to a specific hardware component or software subsystem. All libraries are consumed through a **central hub — `RobotInstances`** — which initializes every global instance required by the robot. The two main entry points, **`striker.cpp`** and **`goalkeeper.cpp`**, each implement their respective game logic by including this hub.

```
Soccer-Vision-2026/
│
├── platformio.ini                  // Teensy 4.1 build config
│
├── lib/                            // Custom libraries
│   │
│   ├── constantes/
│   │   └── constantes.h            // ALL tunable constants & pin definitions
│   │                                  (speeds, thresholds, PID gains, debug flags)
│   │
│   ├── motor/
│   │   ├── motor.h                 // Single DC motor abstraction
│   │   └── motor.cpp               // SetSpeed(), MovePositive/Negative()
│   │
│   ├── motors/
│   │   ├── motors.h                // 4-motor omnidirectional base
│   │   └── motors.cpp              // MoveOmnidirectionalBase(angle, speed, pid)
│   │                                  composes 4× motor instances internally
│   │
│   ├── BNO/
│   │   ├── BNO.h                   // IMU wrapper (Bosch BNO055)
│   │   └── BNO.cpp                 // GetYaw(), SetTarget(), GetError()
│   │
│   ├── PID/
│   │   ├── PID.h                   // Generic PID controller
│   │   └── PID.cpp                 // Calculate(error) → angular velocity output
│   │
│   ├── Camera/
│   │   ├── camera.h                // Serial vision interface
│   │   └── camera.cpp              // Reads ball_angle, ball_distance,
│   │                                  ball_area, goal_angle from Serial1/2
│   │
│   ├── PhotoMux/
│   │   ├── PhotoMux.h              // 16-sensor multiplexed line detector
│   │   └── PhotoMux.cpp            // isLineDetected(side), getAverage(side)
│   │                                  4 mux chips × 4 sides (FRONT/LEFT/RIGHT/BACK)
│   │
│   ├── Kicker/
│   │   ├── kicker.h                // Solenoid kick actuator
│   │   └── kicker.cpp              // update(ballSeen, distance) → pulse pin 32
│   │                                  70ms pulse, 5s cooldown, 70cm threshold
│   │
│   ├── Dribbler/
│   │   ├── Dribbler.h              // Ball dribbler (stub, incomplete)
│   │   └── Dribbler.cpp
│   │
│   └── RobotInstances/
│       ├── RobotInstances.h        // ← CENTRAL HUB: includes all libs above
│       └── RobotInstances.cpp         declares & initializes all global instances:
│                                       bno, pid, motorss, frontCam, mirrorCam,
│                                       phototransistors, kicker
│                                       + initialize_robot()
│
└── src/
    │
    ├── striker.cpp                 // ← YOU ARE HERE (active branch)
    ├── goalkeeper.cpp              // #include "RobotInstances.h" (gets everything)
    │                                  Game loop: search → orbit → align → shoot
    │
    └── test/
        ├── test_motors.cpp         // MoveOmnidirectionalBase() unit test
        ├── test_kicker.cpp         // Solenoid pulse test (pin 32)
        ├── Test_Sensor.cpp         // Raw phototransistor readings per channel
        ├── Test_Multiplex.cpp      // All 4 sides averaged mux readings
        └── I2cscanner.cpp          // Scans I2C bus, prints device addresses
```

## Strategy

The robot deploys two independent behavioral programs depending on the assigned role during a match: **striker** and **goalkeeper**. Each role is implemented as a separate source file and shares the same underlying hardware libraries through the `RobotInstances` hub.

### Striker

The striker's primary objective is to maintain an **offensive position** in the upper half of the field and actively pursue the ball to score. The robot operates across two main states: **searching for the ball** and **executing an attack** when the ball is detected in front.

The robot continuously reads data from both OpenMV cameras. The **frontal camera is prioritized** as its output is more accurate and directly relevant to attack execution. When the frontal camera does not detect the ball, the robot uses the **mirror camera** to determine the ball's angular position and moves towards it with a **±30° offset**. An exception is made when the ball is detected behind the robot — defined as an absolute angle greater than **150°** — in which case the robot repositions at **120°** to achieve a better approach alignment.

When the frontal camera detects the ball and confirms it is within range using **blob area and distance thresholds**, the robot enters **attack mode**. In this state, it aligns the ball and the opponent's goal to a similar angular reference and then drives toward the goal, triggering the **kicker** to shoot.

### Goalkeeper

The goalkeeper follows a **defensive strategy** focused on protecting its own goal. When the frontal camera does not detect the ball, the robot uses the mirror camera to **align its heading** to the ball's angular position. If neither camera detects the ball, the robot **remains stationary** and waits.

When the frontal camera detects the ball, the robot repeats the alignment process. Once the ball is confirmed to be within a **critical proximity threshold**, the robot executes a **kick** to push the ball away from the goal area, reducing the risk of conceding a score.

## Resources
Please visit our [repository](https://github.com/RoBorregos/Soccer-Vision-2026) for a complete look at our work.

## Areas

- [Control](Control.md)
- [Vision](Vision.md)
- [PID](PID.md)
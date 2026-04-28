# Electronics Documentation – LARC Robot 2024

## Introduction

For all the devices that would be used in the final product to interact with the computer, the use of a microcontroller and a microprocessor is necessary. It was decided that a **Teensy 4.1** would be the chosen microcontroller for this purpose, since it has a powerful ARM Cortex-M7 processor and enough digital and analog I/O to manage all required components efficiently. A **Raspberry Pi 5** was used as the microprocessor so that the robot could detect the packages through cameras and allow bilateral communication with the Teensy.

![Teensy 4.1]()  
![Raspberry Pi 5]()

## Connections

The connections were made so that all the sensors and motor drivers were connected to the **Teensy 4.1**, except for the power supplies, which were managed separately to avoid interference and ensure stability.

Two **LiPo batteries** were used:

- The **first battery** is an **11.1V LiPo**, and it supplies the **logical side** of the system. This battery connects to two **voltage regulators** [mini560], which outputs **5V** and **3.3V**. The **5V line powers the Raspberry Pi 5**, while the **3.3V line is used for sensors and actuators**.
- The **second battery** is a **12V 5000mAh LiPo**, used **exclusively for the power side**: **stepper motors, DC motors, and their respective drivers**. This separation helps avoid voltage drops or noise affecting the logic components.

**XT60 connectors** were chosen for all the battery connections, as they provide solid connections and include internal diodes, offering polarity protection in case of incorrect insertion.

![XT60 connector](https://github.com/RoBorregos/RoBorregos-Docs/assets/117100165/41ac4569-05e2-41f2-a3ef-e5b3c1a0e838)

There were also two **LED indicators** for voltage status:
- The **logical side** for the Raspberry Pi, sensors and actuators.
- The **motors side** for the motors and drivers.

![LED Indicators]()

The schematic (designed in **EasyEDA**) reflects the final configuration:

![Schematic 1]()  

## PCB Development

A software tool such as EasyEDA can develop Printed Circuit Boards (PCBs) out of electronic schematics, which opens the possibility to integrate many devices in a single board, without the need of making more physical electrical connections than the ones that are explicitly required. Plus, by making a PCB we can be reassured that there will not be any mistakes like overvoltages or short circuits.

The PCB was designed so that this board could be used modularly, in case any other challenge needed these same connections. Such decision came to mind because many of the components that are used in this PCB are basic to make an autonomous robot work. It was also decided that the PCB would have female pin headers, since it was better to attach the components just by that instead of soldering them directly to the board.

Every voltage via was handled so that no device would be overloaded by an excessive amount of voltage that it could not handle, which is the same case for the microcontroller and the microprocessor. It is important to note that the Raspberry Pi is not being contemplated in the PCB, since it is being powered externally by the same battery as the Teensy.

![PCB LARC 2024](https://github.com/RoBorregos/RoBorregos-Docs/assets/117100165/c14e5f21-d7fe-4891-a089-c3382e36a039)

The final result in Electronics matters can be seen in this picture:

![PCB IRL](https://github.com/RoBorregos/RoBorregos-Docs/assets/117100165/4c70fbd9-e77a-4ee7-a554-d3e90e14f275)

## Modifications

---------------------------------------------------------------------------------------------------------------
# Electronics Documentation – LARC Robot 2025 International

## Introduction
The electronic subsystem of the autonomous coffee harvesting robot uses a dual-processor architecture to balance high-speed perception with precise and deterministic motor control. A Teensy 4.1 handles all low-level tasks, including sensor reading, DC motor regulation, servo actuation, and stepper control. Meanwhile, an NVIDIA Jetson is responsible for high-level operations such as coffee-cherry detection, image processing, and navigation decision-making.
This separation prevents heavy computation from interfering with real-time motor performance.

## Connections 
All sensors, motors, and drivers are connected directly to the Teensy 4.1, while the Jetson communicates through serial protocols for command exchange. To ensure stability and prevent noise-related failures, the system uses two fully independent power circuits.

Two 12V battery packs were implemented:

**Battery 1 – Logic Power (12V)**
Feeds the Teensy and all low-voltage peripherals using mini voltage regulators that output 5V and 3.3V.

-5V powers devices such as the Jetson interface.
-3.3V supplies sensors and other peripherals.

**Battery 2 – Motor Power (12V)**
-Dedicated exclusively to high-power loads:
DC motors, stepper motor with TMC2208, and the servo power rail.
This isolation prevents brownouts, voltage dips, and resets on the logic side.

**XT60 connectors** were used for both batteries to ensure secure, polarity-safe connections during operation.

Two LED indicators were also added to monitor power status:

-One for the logic system (Teensy, sensors, Jetson interface)
-One for the motors and drivers

## Prototype Development
Before integrating the complete system, each subsystem was tested independently using an Arduino Uno. Line sensors were evaluated under different lighting, ultrasonic sensors were verified at multiple distances, and the BNO055 IMU was calibrated for reliable orientation readings.
All actuators were tested individually:

-DC motors with encoders (speed, direction, encoder feedback)

-12V elevator motor (smooth vertical motion)

-NEMA 17 stepper with TMC2208 (step accuracy, current limiting, noise reduction via capacitor)

-Servos (pre-programmed intake movements)

## PCB Develpment
A custom **two-layer PCB** was created using EasyEda to consolidate all electronics into a single integrated platform. The schematic was organized into logical subsystems, and every Teensy pin assignment was verified to prevent wiring errors.

Trace widths were assigned based on signal type:

-0.3 mm for low-current logic traces

-Wider traces (1-2.5 mm)for motor lines, rated up to 2.5 A

Routing minimized electrical noise and crosstalk by keeping power and signal lines separated.
DXF files were used to precisely align the PCB with the robot’s mechanical structure, ensuring compatibility with JST connectors and color-coded cables.

The silkscreen includes clear labels and DXF-based icons for fast debugging and assembly under competition conditions.

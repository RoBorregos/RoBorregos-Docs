# Electronics Documentation – LARC Robot

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


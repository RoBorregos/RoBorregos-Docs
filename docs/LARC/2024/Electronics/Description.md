## Introduction

For all the devices that would be used in the final product to interact with the computer, the use of a microcontroller and a microprocessor is necessary. It was decided that an Arduino MEGA 2560 would be the chosen microcontroller for this purpose, since it has a worldwide platform that fulfills the connections that are needed for all the devices that were mentioned at first (ex. line sensors, camera, etc.). In addition to this, the Arduino MEGA 2560 has a wide number of digital inputs, as well as PWM inputs, which supports the final decision to implement it on the robot. A microprocessor, the Raspberry Pi 4, was used so that the robot could detect the packages through the cameras, and with the purpose that both the Arduino and the Raspberry Pi can communicate bilaterally.

## Connections

The connections were made so that the sensors and motor drivers that were needed are connected to the Arduino, with the exception of the power supplies for the motor drivers, which are powered by different LiPO batteries. With that in mind, the first LiPO battery (11.1 volts) is powering the Arduino through a voltage regulator, which is outputting 5 volts to this microcontroller. The second LiPO battery (11.1 volts) is connected to the wheel motors, and the third one (which has the same voltage) is powering a stepper motor.

The XT60 connectors were chosen for all the input voltages since they internally have diodes, which would help protect the circuit overall in case the current went the opposite way in any moment.

There were also three indicators for some voltage sources, such as the Arduino's converted supply voltage (which is 3.6V in the schematic, but was made 5V at last), the Arduino's not regulated supply voltage (the 22V line in the schematic, which is now 11.1V), and the motors' supply voltage (the 12V line, 11.1V at last).

There are some support devices for the components, like the push button to reset the camera. At first, an OpenMV CamH7 Plus was used, but it no longer turned on, so the team had to switch to normal USB cameras to process all the packages for the challenge. This button would be later used as a reset for the Arduino MEGA instead.

## PCB Development

A software tool such as EasyEDA can develop Printed Circuit Boards (PCBs) out of electronic schematics, which opens the possibility to integrate many devices in a single board, without the need of making more physical electrical connections than the ones that are explicitly required. Plus, by making a PCB we can be reassured that there will not be any mistakes like overvoltages or short circuits.

The PCB was designed so that this board could be user modularly, in case any other challenge needed these same connections. Such decision came to mind because many of the components that are used in this PCB are basic to make an autonomous robot work. It was also decided that the PCB would have female pin headers, since it was better to attach the components just by that instead of soldering them directly to the board.

Every voltage via was handled so that no device would be overloaded by an excessive amount of voltage that it could not handle, which is the same case for the microcontroller and the microprocessor. It is important to note that the Raspberry Pi is not being contemplated in the PCB, since it is being powered by the same battery as the Arduino externally. Another fact to consider is that initially it was intended that the PCB would have three different vias: 22 volts, 12 volts and 19 volts. At last, it was inferred that the best decision would be to use three 11.1 volts batteries.

## Modifications

Other modifications included adding a new voltage regulator for the gripper’s servos, since they needed at least 6 volts to operate. Thus, it was decided that the battery that would supply energy to the Arduino would also power said servos.
One of the difficulties that was encountered throughout the development of this challenge was that both the Raspberry Pi and the TMC2209 driver for the stepper were overheating, so two cooler fans were designed so this could be fixed. Both fans are powered by almost 12 volts (which are being outputted by one of the stepper motor's driver slots).

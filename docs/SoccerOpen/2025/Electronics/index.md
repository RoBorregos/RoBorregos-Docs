# Electronics

## 1. General

### 1.1 Project Structure Overview  

For the RoboCup Junior Soccer Open competition, we developed two identical robots, named Flor and Capullo, sharing the same electronic and mechanical architecture. This strategy allowed us to replicate performance and simplify cross-maintenance between units. The design followed a modular architecture, which enabled us to divide the electronics into separate subsystems with dedicated custom PCBs.

### 1.2 Main Microcontroller

The core of the system is a Teensy 4.1 microcontroller, chosen for its high clock speed (600 MHz), large number of GPIO pins, and excellent multitasking capabilities. This choice allowed us to perform navigation calculations, sensor data processing, and motor control efficiently in real-time.

### 1.3 Custom PCB Design

Each robot includes six custom-designed PCBs, each assigned to a specific task (such as motor control, sensor integration, power regulation, etc.). These boards were designed using EasyEDA, a circuit design platform that allowed us to maintain a clean, compact, and organized layout within the limited space of the robot.

### 1.4 Motor Control System

The movement system consists of four Pololu 4.4 HP 12V motors, each connected to a TB6612FNG driver. These drivers are powered directly with 12V and controlled via 3.3V PWM signals from the Teensy. This configuration ensured fast and precise omnidirectional mobility.

### 1.5 Environmental Sensing

Each robot is equipped with two OpenMV cameras:

* One front-facing camera for ball tracking.

* One camera with a fisheye lens for omnidirectional vision.


For orientation, a BNO055 gyroscope was used, connected via the I2C protocol. Additionally, infrared phototransistor line sensors were mounted on the bottom of the robot to detect field boundaries and prevent the robot from going out of bounds.

### 1.6 Special Actuators

The robot also features specialized actuators:

* **Kicker**: a solenoid mechanism triggered by the Teensy to perform shooting actions.


* **Dribbler**: composed of a DJI 2212 brushless motor and a compatible ESC (Electronic Speed Controller), used to keep control of the ball during movement.


### 1.7 Communication Protocols

The electronic system uses two main communication protocols:

* **I2C**: used for communication between the BNO055 gyroscope and the microcontroller.

* **UART**: used to interface the Teensy with the OpenMV cameras.

## 2. Power Supply

### 2.1 Main Power Source

The robot uses a 3-cell (3S) 1500 mAh LiPo battery as the primary power source for the main board. This battery is connected to a XI005 voltage regulator, which steps down the battery voltage (nominal ~11.1V) to 5V. This 5V output powers the main board and is also connected to a second XI005 regulator, which further reduces the voltage to 3.3V for digital signals and low-power sensors connected to the Teensy microcontroller.

The same battery voltage is also connected to a XI4015E1 buck regulator, which regulates the voltage down to a stable 12V, used to power the four drive motors. In total, the system uses three voltage regulators: 3.3V, 5V, and 12V.

### 2.2 Separate Kicker Power System

To isolate the high-voltage kicker module from the main system, a separate 1-cell (1S) 1500 mAh LiPo battery is used exclusively for the kicker board. This separation ensures that voltage spikes from the kicker (which operates up to 48V) do not affect the main board or the Teensy microcontroller.

### 2.3 Safety Features

Several safety measures were implemented:

* On the kicker board, optocouplers and diodes are used to isolate the Teensy from high-voltage paths (up to 48V), preventing direct electrical contact.

* On the main board, protection diodes are installed at each power input line connected to the voltage regulators to prevent reverse polarity or accidental shorts.


### 2.4 Power Control

The robot includes two physical switches:

* One switch controls power flow from the battery to all voltage regulators.

* The second switch selectively enables the 12V line to the motors, allowing programming or debugging of the robot without activating the motors, ensuring safety during development.


### 2.5 Charging Convenience
The battery system is designed for easy disconnection, allowing the batteries to be removed and charged externally without disassembling the robot.



## 3. PCB Design

### 3.1 Line Detector Boards

For the bottom line sensor modules, we developed custom PCB designs to meet specific size and shape constraints and to achieve higher detection accuracy compared to commercial boards. Each robot includes three general-purpose detector boards and one front-facing board. The front-facing board features a slightly different layout to accommodate space constraints caused by the kicker and dribbler mechanisms.

Each general board is equipped with eight AP2012P3C-P22 phototransistors, which are highly sensitive in the 850–900 nm infrared range, paired with eight SFH 4656 infrared LEDs, which emit light primarily in the 800–900 nm range. This wavelength match ensures a reliable and noise-resistant reading, since the green color of the field absorbs IR, while white lines reflect it, producing a sharp contrast in sensor readings.

To minimize the number of analog pins used on the Teensy, each board includes a 74HC4051MTR multiplexer. The PCB exposes two sets of output connections:

* One for the multiplexed output and control lines (S0, S1, S2, OUT).

* Another set for four individual analog outputs, allowing optional direct reading.

The front-facing board operates similarly but includes seven phototransistors and seven IR LEDs instead of eight, to reduce physical size.

![Electronic schematic of the general line detector board](/docs/assets/soccer/Electronics/General_line_detection_board.jpg)
*Electronic schematic of the general line detector board*

![Electronic schematic of the front line detector board](/docs/assets/soccer/Electronics/Front_line_detection_board.jpg)
Electronic schematic of the front line detector board

### 3.2 Main Board (4 Layers)

The main PCB was also designed in-house using EasyEDA, and it features a 4-layer layout. It serves as a modular baseboard with a female header socket for the Teensy 4.1, allowing easy installation and replacement. The board integrates connections for all major components:

* Line sensor inputs

* Motor outputs

* Gyroscope (BNO055)

* Solenoid control and kicker board connection

* Dual OpenMV cameras

* Dribbler motor and ESC


All peripheral devices are connected using 2, 3, or 4-pin JST connectors, and the motor drivers (H-bridges) are also modular, connected via female headers. This modularity improves maintainability and debugging efficiency.

![Main board electronic schematic with Teensy 4.1 integration](/docs/assets/soccer/Electronics/Main_board_electronic.jpg)
*Main board electronic schematic with Teensy 4.1 integration*

### 3.3 Kicker Module Board

The kicker board is powered by a separate 3.3V input from a dedicated 1-cell LiPo battery. It includes an XL6019E1 step-up converter, which boosts the voltage to 48V using inductors and capacitors. This high voltage is stored in two parallel 4700 µF capacitors, ready to discharge through the solenoid.
To safely interface with the Teensy, the activation circuit employs CYPC817(C-TP2) optocouplers. When the Teensy sends a control signal, the optocouplers switch and allow the stored 48V to flow through the solenoid. This isolation protects the microcontroller from direct contact with high-voltage circuits.

![Electronic schematic of the kicker module](/docs/assets/soccer/Electronics/Kicker_module.jpg)
*Electronic schematic of the kicker module*

### 3.4 Design Considerations
All PCBs were designed using EasyEDA, with careful attention to:

* Minimizing layer transitions (vias)

* Proper separation of high-voltage and low-voltage traces

* Sufficient clearance and routing space for high-current paths

The final PCBs were fabricated by JLCPCB, chosen for their cost-effectiveness.

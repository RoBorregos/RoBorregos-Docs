# Main Board

This section details the design decisions, routing rules, and power management of the robot's main board.

## 1. Design and Electronic Noise Control
To ensure the stability of the **Teensy 4.1** microcontroller, various electromagnetic noise mitigation techniques were implemented:

* **Ground Pours:** Massive ground planes were created to absorb interference and facilitate current return.
* **Decoupling Capacitors:** These were placed strategically at:
    * Voltage regulator outputs.
    * Motor driver signal outputs.
    * Teensy power input.
    * *Objective:* To filter voltage spikes that could cause unexpected reboots or damage to the main board.
* **Protection Diode:** A diode was included in the power supply line. This is **fundamental** to protect the Teensy, preventing it from receiving dangerous voltages and allowing us to avoid physically cutting the internal power trace (VUSB) when connecting it to the battery and the USB simultaneously.

---

## 2. Routing and Trace Width
The design of the traces and vias (holes that go through the PCB layers) was defined to avoid thermal "bottlenecks" and copper deterioration due to excess current. 

### Signal Traces (0.25 mm to 0.40 mm)
* **Use:** SPI, I2C, UART communication, and all sensor signals.
* **Reason:** Since these traces only transmit data (3.3V logic) and carry minimal currents, they do not need to be thick. Using 0.25 mm allows for dense routing between component pins without causing short circuits, optimizing PCB space.

### Power and Motor Traces (1.0 mm to 1.5 mm or wider)
* **Use:** VIN lines, 5V, driver outputs to the motors, regulator outputs, and direct battery connections.
* **Reason:** Whole amperes flow through here. If a thin trace (0.25 mm) is used for a motor, the trace will act as a fuse: it will heat up and melt, breaking the board. For currents higher than 1 or 2 Amperes, using between 1.0 mm and 1.5 mm is the minimum and safest approach.

---

## 3. Voltage and Logic Management
Given that the Teensy 4.1 operates strictly at **3.3V** and components like the **Pixy2** or the **ATMEGA** can handle **5V** signals, the use of intermediary hardware is mandatory:

* **Level-Shifter (Logic Level Converter):** Implemented to translate 5V signals to 3.3V and vice versa, preventing voltage issues.
* **Warning:** Attempting direct 5V communication to the Teensy without this component is highly dangerous and will instantly burn the microcontroller.
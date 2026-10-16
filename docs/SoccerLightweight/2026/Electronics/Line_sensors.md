# Bottom PCBs (Phototransistors)

## 1. Components and Decisions

* **Component Selection:** The TEPT5700 sensors were used for their good light response; this provides a wide reading range and they are easy to manipulate in code.
* **LEDs:** It is recommended to further investigate the color of the LEDs to be used. It was identified that red and white LEDs are good alternatives, but it will depend on calibration and testing to see which of the two works best.
* **Multiplexers (74HC4051):** These were used to expand the analog readings of the sensors to pins 15, 16, and 17 of the microcontroller. **Important note:** The packaging type (footprint) of the multiplexer is crucial when ordering components; pay close attention to the specifications and packaging dimensions so that it exactly matches the board design. Related to this, verify the footprints to be used in the creation of the PCBs multiple times, ensuring there is adequate space for each component to be ordered.

---

## 2. Voltage and Logic Communication

* **Safety Warning:** All communication signals with the Teensy 4.1 must be strictly at **3.3V**. Failing to respect this logic level and attempting to send 5V signals from the sensor board is a highly dangerous practice that can damage the microcontroller.

---

## 3. Component Testing

* **Resistor Calibration:** The resistor values for both the LEDs (to define brightness) and the phototransistors (to define sensitivity) **must be exhaustively tested on a breadboard and with code first**. Making modifications or replacing resistors on the bottom plates when everything is already soldered adds unnecessary damage to the copper traces and severely affects the precision of the readings.

---

## 4. Power Control (MOSFET)

* **Use of MOSFET:** A circuit with a MOSFET was implemented to turn the LED array on and off via code. From an electronics standpoint, it is a great idea, as it grants total control over the robot to save battery and allows testing various functions. However, this season we did not reach the point of exploring and fully exploiting all its uses. It is recommended that new members carefully analyze if they will actually need this functionality, as it requires an extra communication connection to the main board.
# Electronics Documentation – LARC Robot 2026

## Erick Flores

Basically, the system is divided into 3 parts:

1. **Power section:** motors and drivers.  
2. **Logic section:** sensors and Teensy.  
3. **Computation/vision section:** Raspberry Pi 5 and cameras.  

Each one has its own battery. The logic section must run at **3.3 V**.

---

## PCB

Since multiple sensors were used, I considered using **2 PCBs**: the main board and the multiplexer board. The objective was that, regardless of where the sensors or the main board were placed, the secondary board could be positioned as close as possible to the largest number of sensors, which in this case were located underneath the robot. This way, the data cables are reduced mainly to the multiplexer connections.

This helps reduce the amount of wiring compared to connecting everything directly to the main PCB. For example:

- **Analog multiplexer:** approximately 5 cables.  
- **2 QTR-8A connected directly:** approximately 16 cables.  
- **VLX with multiplexer:** 2 cables, compared to 16 cables if connected directly.  

I recommend adding extra pins even if they are not going to be used. For example, on the I2C multiplexer only 4 ports were initially planned, but I added all 8 available ports. If they are needed in the future, they are already available. Leaving them unused does not affect functionality; they only need to be covered with electrical tape to prevent debris from entering.

I used **EasyEDA Standard**, since the 2025 design was used as a base. It took approximately **4 days** to complete, including wiring everything on a breadboard.

I added **stitching vias**, which reduce impedance and help minimize electrical noise.

Shipping took approximately **1 week**.

**JST connectors** were used for the sensors, while the modules were connected directly using **female headers**. JST connectors require crimping and wiring. **22 AWG silicone wire** was used, although I recommend using **22–20 AWG** wire.

Soldering each PCB took approximately:

- **Main PCB:** 5 hours.  
- **Small PCB:** 2 hours.  

This was mainly due to organizing components and buying a few missing parts.

### EasyEDA Links

- **Multiplexers:** <https://oshwlab.com/erickflowers0969/muxxboard>  
- **Main PCB:** <https://oshwlab.com/erickflowers0969/larc26_pue>  

---

## Batteries

A total of **3 LiPo batteries** were used to power the system:

1. One for the logic section.  
2. One for the motors.  
3. One for the external computer.  

In all cases, a regulator should be used. However, I did not use a regulator for the motors due to time constraints and to avoid modifying the existing system, although using one is recommended.

All batteries were **3-cell (3S)** batteries, since they were connected to regulators and this helped maintain a standard configuration.

Two **6000 mAh 3S batteries** were purchased for the motors and the computer.

The original plan was to keep the logic battery separated into 3 regulators:

- **2 Mini 560 regulators at 3.3 V:** for all sensors and multiplexers.  
- **1 Mini 560 regulator at 5 V:** dedicated to the Teensy.  
- **1 XL4016 regulator with up to 8 A output:** dedicated to the servos.  

This setup was proposed to avoid fluctuations by separating the servo regulator from the Teensy regulator. Connecting them together is not recommended.

However, my main PCB was not ultimately used. The first time I tested it on screws, it burned out, and the second time I could not find the issue. Because of this, the 2025 board skeleton was reused with modifications. Essentially, the power section connected directly to the motor battery remained unchanged from the 2025 version.

For the sensor section, only the predefined **3.3 V regulator output** was used. I also connected a **5.1 V regulator** directly to the Teensy.

Another regulator from the butterfly board to the servos was originally planned. However, during testing, using 2 servos simultaneously caused the Teensy to shut down. To solve this, I connected the servos to the Raspberry Pi regulator, since it can provide up to 8 A output and was only powering the Raspberry Pi.

For the Raspberry Pi, the second **6000 mAh 3S battery** was used together with the **XL4016 regulator**. To power it, I cut a coaxial cable rated for up to 5 A, since according to the specifications that is approximately what it consumes.

Because it was a coaxial cable:

- The outer part was **ground**.  
- The inner part was **positive**.  

I insulated these wires so they could be connected to the regulator. First, I insulated the ground using heat shrink tubing, then stripped the positive wire and connected both to the regulator outputs. Finally, the USB-C cable was connected to the Raspberry Pi.

The two servos used in the robot were also connected to this same output, since they also required 5 V.

The **Teensy 4.1** logic operates at **3.3 V**. This means the inputs can only safely measure 3.3 V. For example, ultrasonic sensors operate at 5 V, meaning the signal sent back to the Teensy also remains at 5 V. If the IR sensors had been powered at 5 V, their output to the Teensy would also have been 5 V, which could damage it. I am not exactly sure whether the entire board would stop working or if only the affected pin would be damaged. Because of this, all sensors were powered at **3.3 V**.

---

## Adverse Situations

While adapting the I2C bus, I directly soldered two wires onto the Teensy where the top headers are located. In this configuration, the system did not work.

Later, I cut the wires and connected them from the back side, but they remained like two exposed antenna wires. The I2C bus only worked correctly once they were completely removed.

Recommendations:

- Do not test PCBs on top of screws.  
- Always test PCBs and electronics using a power supply.  
- When a short circuit is detected, the power supply automatically shuts down.  

---

## Sensors

The programming team and I discussed the approach we wanted to take for the challenge, so several sensors were requested. I had to investigate how to integrate all of them together.

The sensors ultimately used were:

| Qty | Sensor | Notes |
|---:|---|---|
| 4 | TCRT5000 Infrared Reflective Optical Module | Connected digitally, since analog mode provided a very low usable range. |
| 4 | Adafruit VL53L1X | Better than the L0X. Must be used with a multiplexer. Reliable. |
| 1 | PCA9548A | Required to connect up to 8 VL53L1X sensors because they share the same I2C address. |
| 1 | BNO055 | Connected directly to the I2C bus to avoid routing through the multiplexer. |
| 1 | 74HC4067 | Analog multiplexer for the QTR-8A sensors. EN connected to GND to keep it always enabled, and SIG connected to an analog pin. |
| 1 | Chinese QTR-8A | It is recommended to switch to Pololu. In use it was reliable, although during testing it sometimes showed a “low range.” |
| 4 | Black SG90 servos | Better than the blue versions because they use metal gears. Be careful with power delivery. PWM was connected directly to the Raspberry Pi to reduce latency. |

For the I2C sensors, the datasheets recommended adding ceramic capacitors to the power supplies.

It was also recommended to add **4.7 kΩ resistors** from SDA to 3.3 V and from SCL to 3.3 V. I did not add them because, when using the modules, I measured the resistance and they were already present. Therefore, I left them out and everything worked correctly.

The motor drivers basically use 3 pins for each motor:

- **ENA**  
- **ENB**  
- **PWM**  

These connect to the driver, and from the driver to the motor there are only two wires:

- Black.  
- Red.  
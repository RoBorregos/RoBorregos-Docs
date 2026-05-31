# Vision: Pixy2 Camera

## 1. Communication Protocol Comparison
Three options for communicating the Pixy2 with the Teensy 4.1 were analyzed. The decision is crucial to avoid slow response times (lag).

| Protocol | Advantages | Disadvantages |
| :--- | :--- | :--- |
| **I2C** | Allows daisy-chaining more sensors on the same bus. | Very slow (100-400kbps). Creates bottlenecks with the amount of vision data. |
| **UART (Serial)** | Very stable point-to-point connection. Easy to program using serial code. | Takes up a full hardware serial port. |
| **SPI (Recommended Option)** | Maximum speed (up to 2Mbps). Zero lag. It is the camera's native protocol. | Requires specific wiring to pins 11, 12, and 13. |

---

## 2. Power Considerations
I have never been able to power the Pixy2 from the Teensy's 3.3V output. Its consumption (~140mA) is very high and will burn the internal regulator. It must always be connected directly to the VIN (5V) pin.

## 3. Connection Warnings
Take into account the connections on the camera, as an adapter must be ordered because the cables used to connect it tend to come loose and cause many short circuits.
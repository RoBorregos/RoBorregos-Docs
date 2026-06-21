# IR Ring (Infrared Sensors)

## 1. Sensor Selection

* **TSSP4038 Sensors (Chosen):** These modules are designed for light barriers and continuous proximity detection. They do not block the continuous infrared signal emitted by the competition ball.
* **Alternative to the chosen sensor:** Review other options besides the TSSP4038 because the ball changed this year and is much smaller, and the IR light is only emitted when the ball moves. Therefore, the best option might be to set the ring aside and try another strategy to detect the ball.

---

## 2. Physical Challenges

* **Blind Spots:** Since the ball is smaller, there was a certain distance at which the ring did not detect the ball because it was blocked by the robot's chassis. Here, the camera option is good to cover that angle that the ring does not detect. 
* **Ring Size:** Another obstacle was the size of the ring, which was increased in our year seeking to have more IR sensors and a wider field of vision. We saw that most teams with good ball tracking opted for Lego sensors or avoided large rings, keeping them quite close to the robot's body.

---

## 3. Communication Issues

* **ATMEGA Role:** It is important to mention that the ATMEGA used in the ring was merely to avoid using up all the inputs on the Teensy 4.1. This means that the Arduino is intended only to receive the data and pass it immediately to the Teensy, so that the calculations are performed and the code is executed there.

---

## 4. Recommendations

* **Component Footprints:** Regarding communication, it is fundamental, as I already mentioned, to have the used footprints and ordered packages well-defined to avoid connection problems. 
* **Status LED:** It is also highly recommended to incorporate an LED that indicates the state of the Arduino, as this will make it much easier to debug any problem with the ring.
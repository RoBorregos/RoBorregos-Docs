# Robot Features
Our robot system has been designed with several key features that improve flexibility, modularity, and precision in its operation, enabling easier adjustments and enhancements for future developments.

### **1.S tandardized Speed Range**

To ensure consistent control and make the system more modular, we standardized the robot’s speed by setting it within a range of 0 to 1. This approach allows for easy adjustments to the speed control algorithm and ensures compatibility across different microcontrollers, simplifying any potential hardware changes.

### **2. Moving Average for Sensor Data**

To improve sensor accuracy and stability, we implemented a moving average algorithm that processes real-time data from sensors, such as the phototransistors, camera, and ultrasonic sensor. This algorithm smooths out noise and fluctuating values, providing a more reliable reading. The moving average is calculated as an average of moving readings of the sensor, where each new value is combined with a fixed number of previous readings. By continuously updating this average as new data arrives, the algorithm reduces the impact of sudden spikes or drops, making the sensor output more consistent and suitable for precise decision-making in dynamic environments.

The structure of our code for the moving average is the following:

```cpp
// Read sensor as usual
uint16_t value = readSensor(); 

// Update circular array 
values_array[*index] = value; // Save value in array
*index = (*index + 1) % kMovingAverageSize; // Increase the index in a circular manner

// Calculate moving average
uint32_t sum = 0;
for (int i = 0; i < kMovingAverageSize; i++) {
    sum += values_array[i];
}
uint16_t moving_average = sum / kMovingAverageSize;
```
`kMovingAverageSize` is a constant you need to establish. A small moving average size (3 to 5) is faster but does not provide values as accurate as a larger moving average size, which helps achieve more precise results but is slower to respond.



### **3. Dual-Ring Phototransistor Array with Multiplexer**

We use a dual-ring phototransistor array for line and light detection, optimized with an 8-to-1 multiplexer for each board. This setup allows for a larger number of phototransistors to be integrated into the robot, enhancing its ability to detect lines and light changes from multiple directions.

- **Multiplexer:** Each board is equipped with an 8-to-1 multiplexer, enabling the system to read data from up to 8 phototransistors with a single input, reducing the complexity of wiring and improving sensor efficiency.
- **Dual-Ring Setup:** The dual-ring configuration of the phototransistors ensures broader coverage for line detection, allowing for more precise tracking and better response to dynamic field conditions.

### **4. Three-Board Sensor Configuration**
To enhance detection accuracy and redundancy, the robot is equipped with three sensor boards positioned around the robot. Each board includes an array of phototransistors and a multiplexer, working together to provide comprehensive coverage of the robot's surroundings, allowing it to react quickly and effectively to changes in the environment. 
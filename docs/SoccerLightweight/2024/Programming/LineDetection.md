# Line Detection

In the Soccer Lightweight project, detecting the field's lines is essential for maintaining the robot's position and ensuring it stays within the playing boundaries. The line detection system differentiates between the white lines on the field and the green background, enabling precise navigation.


## Line Detection Strategy

The line detection strategy employs a combination of analog sensors and a multiplexer to read values from different parts of the field. The key components and steps in this strategy include:


- **Analog Sensors**: Multiple analog sensors are positioned around the robot to read the field's color, distinguishing between white lines and the green background.
- **Multiplexer (MUX)**: A multiplexer switches between different sensor inputs, allowing the robot to monitor several sensors using a single analog input pin on the Arduino.
- **Threshold Values**: Threshold values are set to differentiate between white lines and the green field, determining whether the sensor is over a line or the field.

## Implementation

The implementation involves reading sensor values and comparing them to predefined thresholds. The key functions responsible for line detection are:


### muxSensor()
Reads values from the multiplexer and direct sensor pins, comparing them to threshold values to determine if they are over a white line or green field.

### calculateDirection()
Determines the direction of the detected line and adjusts the robot's heading accordingly.

## Testing and Calibration
The line detection system underwent extensive testing and calibration to ensure accuracy. Thresholds for distinguishing between white and green were fine-tuned based on experimental data, ensuring the robot can reliably detect lines under various lighting conditions and on different field surfaces.




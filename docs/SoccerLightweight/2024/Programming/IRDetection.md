# IR Detection

In the Soccer Lightweight project, the precision of robot movement and positioning on the field was really important for successful gameplay. Essential to this was accurately determining both the angle and distance to the ball. To achieve this, an IR ring utilizing 12 TSSP-58038 IR receivers was designed.


## IR Ring Design and Functionality

The IR ring consists of 12 TSSP-58038 IR receivers arranged in a circular ring. This configuration enables the robot to detect the angle of the ball relative to its position and estimate the distance to the ball. The primary objectives of the IR detection system include:


- **Angle Detection**: To determine the direction of the ball.
- **Distance Measurement**: To estimate how far the ball is from the robot.

## Mathematical Calculations

The IR detection system processes and filters the signals received from the IR sensors to determine the ball's angle and signal strength. Referencing the Yunit team's research from 2017, the calculations can be summarized as follows:

- **Signal Processing**: The IR sensors detect the ball and send angle and strength data to the Arduino.
- **Filtering**: We apply an exponential moving average (EMA) filter to smooth the data, ensuring stable and accurate readings.
- **Angle Adjustment**: The raw angle data is adjusted to account for any offsets and converted to a 0-360 degree format for easier interpretation.
- **Strength Calculation**: The strength of the signal indicates the distance to the ball, with stronger signals meaning the ball is closer.

## Code Implementation

Here is a brief overview of the core code responsible for processing the IR data:

```cpp
void IR::updateData() {
    if (Serial3.available()) {
        String input = Serial3.readStringUntil('\n');
        if (input[0] == 'a') {
            angle = input.substring(2).toDouble() + offset;
            filterAngle.AddValue(angle);
        } else if (input[0] == 'r') {
            strength = input.substring(2).toDouble();
            filterStr.AddValue(strength);
        }
    }
}

double IR::getAngle() {
    return filterAngle.GetLowPass();
}

double IR::getStrength() {
    return filterStr.GetLowPass();
}
```
This code reads data from the IR sensors, applies filtering to the angle and strength values, and adjusts them for precise detection.

## Implementation and Testing

Comprehensive testing was conducted to calibrate the system and verify its accuracy. By employing TSSP-58038 IR sensors and advanced filtering techniques, we achieved reliable and precise ball detection, enabling the robot to execute complex movements and strategies with effectiveness.
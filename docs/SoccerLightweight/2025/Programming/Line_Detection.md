# Line detection - Phototransistors and Multiplexer

Phototransistors detect the field lines. These sensors help the robot identify when it's crossing a white line, allowing it to correct its path and stay within the field during a match.

Phototransistors measure reflected light from the ground:

- High values: Bright surface (white line)
- Low values: Dark surface (green area of the field)

When we started developing the robot, we tested PCBs that covered a small area. However, due to the relationship between the sensor readings and the robot's movement, it couldn’t correct itself in time. That’s why we decided to implement a double ring of phototransistors to achieve a wider reading range.

## Strategy

During testing, we noticed that calibrating the threshold values was very difficult to achieve.
That's why we implemented a strategy where we created a `PhotoCalibrationOnLine()` method. This code was run during calibration to read the values when the phototransistors were detecting a white line.

Then, during the match, the `CheckPhotosOnField()` method continuously read the phototransistor values with a **moving average** and compared them with the calibration values to determine whether the robot was on the field or over a line.

A struct was created to store a ***boolean*** value indicating whether the robot is over the line, which can be checked in `main.cpp`, and a ***correction angle*** indicating the direction the robot should move to correct its position if it is on a line.

```cpp
struct PhotoData {
    bool is_on_line;
    int correction_degree;
};
```
To standardize the reading process for all phototransistors, a `switch` statement was implemented to apply the corresponding logic depending on which sensor was being read.

```cpp
enum class Side {
    Left,
    Right,
    Front
};
```

## Multiplexer

A multiplexer is a device that allows multiple sensors to be connected to a single input of the microcontroller, selecting one at a time to be read.

It is programmed by selecting the channel (sensor) to be read, and then its value is obtained just like any other sensor directly connected. This saves pins and makes it easier to manage multiple sensors efficiently.

To make the code more modular, a dedicated library was created for the multiplexer, which includes a method to select the channel to read.

```cpp
float MUX::readChannel(uint8_t channel) {
    // Set the selection pins to choose the channel to be read.
    digitalWrite(s0_, channel & 0x01);
    digitalWrite(s1_, (channel >> 1) & 0x01);
    digitalWrite(s2_, (channel >> 2) & 0x01);
    delayMicroseconds(5); // Allow signals to settle
    // read selected channel
    return analogRead(signal_pin_);
}
```

In the `photo` library, an object is created for each multiplexer in the robot, allowing the readings from each group of phototransistors to be separated based on their position (left, right, front).

## Data processing

For each side, all sensors were read, and an average was calculated. A moving average of each average value was then used to continuously update the phototransistor readings. Each value was compared against the threshold constant established during the calibration process.

To determine whether the robot is on a line, we compare the most recent sensor value with the average of the moving average and the calibrated threshold. If the current value is greater than that midpoint, we assume the robot is on a white line. The condition sets a dynamic threshold halfway between the current trend and the calibrated reference, providing a more robust way to detect line crossings even with slight variations or noise.

```cpp
bool is_on_line = value > ((moving_average + calibration_line) / 2);  
```

Finally, in the `main.cpp` file, we add a condition to check the boolean value and execute a correction movement if the robot is on a line. Example:

```cpp
uint16_t valueleft = photo.ReadPhotoWithMUX(Side::Left);
    if (photoDataLeft.is_on_line) {
        motors.MoveOmnidirectionalBase(photoDataLeft.correction_degree, 1, 0);
        delay (kLineCorrectionTime);
        motors.StopAllMotors();
    }
```

## Considerations and recomendations

Correct robot line correction is essential for good performance in the contest. That’s why we recommend focusing on making these function work perfectly.

Here are some recomendations:

1. Instead of taking an average of the sensors on each side, ***process each sensor reading individually*** to improve correction time.

2. Using delay is not the best option for correction timing, but we still use it because, during correction, no other sensor functions are critical. Still ***explore options using `millis()`***.

3. When powering the robot, phototransistors might give false readings that can affect the robot's movement. To fix this, we added a condition that makes the robot wait before starting to read the sensors:

```cpp
j = 1;
if (j == 1){
    motors_start_time = millis();
    j = 2;
}

if (millis() - motors_start_time >= 1500){
    // Add the line correction condition inside this block
}
```

4. To increase the difference between the values on green and white areas, consider ***squaring the sensor values***.

5. Consider ***using green LEDs*** for illumination. Since the field is green, it will reflect less light, potentially increasing the contrast between surfaces.

6. Research the use of ***interrupt pins*** for faster correction time and more efficient functionality.
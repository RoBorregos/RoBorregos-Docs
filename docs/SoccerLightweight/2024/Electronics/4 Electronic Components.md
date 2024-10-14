# Electronic Components

## Microcontroller
For the microcontroller, the Arduino Mega Pro was chosen. This is due to its ease of use in electronics and programming. The Arduino Mega Pro microcontroller has 54 digital pins, 15 digital ports with PWM output, 16 analog pins, among many other things.

## Sensors
For the orientation of the robot, we use a BNO055 sensor which has very good performance and accuracy. It is also possible to connect this sensor via I2C to the microcontroller.
For the detection of lines, TEPT5700 phototransistors were used since the range in which they work does not include infrared light, which is crucial so that it does not interfere with the signals from the IR ball.

Digital IR receivers TSSP58038 were used to detect the IR signals emitted by the ball and a custom PCB was also designed. The IR ring is made up of 12 IR receivers, and an Atmega328p was used for processing and vectoring the infrared signals.

## Drivers and motors
For the motion section, we use HP motors at 2200 RPM with TB6612FNG drivers. For this, we bridge the input and output signals to obtain up to 2.4 continuous Amps and up to 6 peak Amps since the motors require too much current.

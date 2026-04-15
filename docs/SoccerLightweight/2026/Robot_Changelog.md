# Robot Changelog
Several improvement were made from the previous version. 

## **Hardwarewise**
- Usage of **Teensy 4.1** _(previous gen: Arduino Mega Pro & ESP32)_
- Weight increment to 1.350 kg _(previous gen: 0.900 kg)_
- Usage of BNO085 _(previous gen: BNO055)_
- Usage of TSSP4038 receivers _(previous gen: TSSP58038)_
- Usage for red ultrabright LEDs for striker _(previous gen: white ultrabright LED)_
- Added 3 extra IR receivers
- Added kicker to defender
- Added three power switches, one to pass current to the battery, another one to initialize logic, and the last one to initialize motors. 

## **Drafted hardware changes**
- Added dribbler to striker. Removed due to phototransistor space conflict
- Attempted to replace all LEDs. Removed due to unsufficient material

## **Softwarewise**
- Completely new implementation of `Photos` and `MUX` libraries. The method to detect line was formalized, and it can now compare between each and every one photoresistor rather than doing an average.
- Partial refactoring of `sensorControl` (formerly named: `sensor_control`) to include 15 IR receivers. Modified angle retrieval method, it now uses vector components with SMA to act as the kernel in order to get the final angle.
- Completely new implementation of `PID`, constructor now allows to set a minimum and maximum PWM with a deadband in order to prevent for the motors to get less PWM that what they need in order to move.
- Added a robot class in order to initialize all sensors on a single environemnt, not needing to worry for proper library imports in tests.
- Changed BNO library in order to work with BNO085

## **Drafted software changes**
- Made binarySerializationData for a more proper form of communication between AtMega & Teensy. Unused due to only returning a single variable
- Made an IMU library as a temporary measure in order that works similarly to a BNO, with the only difference being that it does not contain a metallic anchor. Unused in favour of using BNO085

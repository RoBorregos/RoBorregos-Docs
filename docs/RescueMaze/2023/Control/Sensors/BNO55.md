# BN055

The BN055 sensor is an absolute orientation sensor that provides useful information to determine the robot's state. In Rescue Maze, it was used to make sure the robot advanced straigth, perform 90-degree turns,
and detect when the robot was moving through obstalces or ramps.

Product link: [https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview](https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview)

Library used: [https://github.com/adafruit/Adafruit_BNO055](https://github.com/adafruit/Adafruit_TCS34725)

The sensor has multiple modes (see [Adafruit_BNO055.h](https://github.com/adafruit/Adafruit_BNO055/blob/master/Adafruit_BNO055.h) lines 61-75). The differences between the [modes](https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf#page=20&zoom=100,90,342) are the sensors used to calculate the orientation and the output information available. The mode used was:

```cpp
adafruit_bno055_opmode_t mode = OPERATION_MODE_IMUPLUS;
bno.begin(mode)
```

## Calibration

The sensor needs to be calibrated before it can be used. The calibration process is explained [here](https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/device-calibration). The sensor must be moved in specific directions to
calibrate each sensor. The accelerometer and gyroscope can be calibrated using preloded offsets, but the magnetometer needs
each time the sensor powers off. To obtain the offsets for calibration, the [following code](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/navSensors/other/SaveBnoCal.ino) was used. Once the sensor is calibrated, the program prints the offset values to serial. An example of how to load these values to the BNO055 can be found [here](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/navSensors/main_code/BNO.cpp) (See BNO::restoreCalibration).
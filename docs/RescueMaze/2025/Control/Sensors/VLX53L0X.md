# VL53L0X

The VLX53L0X is a time of flight distance sensor. In Rescue Maze, it was used to detect walls and robot
displacement.

Product link: [https://www.adafruit.com/product/3317](https://www.adafruit.com/product/3317)

Library used: [https://github.com/adafruit/Adafruit_VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X)

The sensor has multiple modes (see [Adafruit_VL53L0X.cpp](https://github.com/adafruit/Adafruit_VL53L0X/blob/master/src/Adafruit_VL53L0X.cpp) lines 210-219). 

## Considerations

While this sensor gives accurate readings, it is not very reliable for long distances. Specifically, its maximum range
is ~1.3 meters and its acurracy decreases the further the object is. In some scenarios, the sensor wasn't reliable to
measure robot displacement. That's why I recommend the black VL53L0X which I tested and it reached a longer distance than the blue one.
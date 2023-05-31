# Sensor Overview

The following sensors were used in Rescue Maze 2023:

- Time of flight distance sensor: [VLX53L0X](./VLX53L0X) (Adafruit)
- RGB Color Sensor: [TCS34725](./TCS34725) (Adafruit)
- Absolute Orientation Sensor: [BNO055](./BNO055) (Adafruit) 

## Using i2c devices

i2c is a protocol used by many of the sensors used in Rescue Maze. The i2c protocol is a serial communication
protocol that allows multiple devices to be connected to the same bus. The advantage of this protocol is that
devices only need two wires to communicate (SDA and SCL, in addition to GND and VCC). The disadvantage is
that each device needs a unique address to be able to communicate with the master device (in this case, the Arduino).

However, since many devices of the same type are used (in the case of Rescue Maze, 4 VLX53L0X distance sensors),
then a multiplexor is needed to avoid address conflicts. The multiplexor connects to the i2c bus and allows the 
master device to select which device it wants to communicate with. The devices with the same address should be 
connected to different multiplexor channels.

## Sensor abstraction

In order to abstract the use of sensors, a class Sensors.h was created. This class is in charge of initializing
the majority of the sensors and providing a clean interface to use them. Instead of accessing the sensor data
directly, it is best to do so through the sensor class. That way, filters could be applied more easily and if the
type of sensor changes, the only thing that needs to be changed is the sensor class.

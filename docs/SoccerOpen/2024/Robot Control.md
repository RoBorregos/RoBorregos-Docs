### Kinematic

For robot control we decided to create our motor libraries following this categories: 

>> 📁 motor

>> 📁 motors

Below is a UML diagram showing the relation between both classes and their interaction: 

![UML Diagram for motor classes](https://i.ibb.co/ctWGs3G/Motors-UML.png)

All of our kinematic logic is found in the Motors class, containing two relevant methods 

```
MoveMotors(int degree, uint8_t speed)
MoveMotorsImu(int degree, uint8_t speed, double speed_w)
```

For both methods the relation in kinematic includes the following equations: 

```
float m1 = cos(((45 + degree) * PI / 180));
float m2 = cos(((135 + degree) * PI / 180));
float m3 = cos(((225 + degree) * PI / 180));
float m4 = cos(((315 + degree) * PI / 180));
```
Which models the robot below: 

![Robot wheel model](https://i.ibb.co/qyBbGWj/robot.png)

For the case of using the IMU sensor, we implemented a omega PID controller to regulate the robot's angle while moving to an specific direction. We also implemented a traslational PID
to regulate speed when approaching the ball. To simplify this we created a PID class to make our code reusable. 

### IMU Sensor

For IMU sensor we utilized the [Adafruit_BNO055](https://github.com/adafruit/Adafruit_BNO055/tree/master), we also implemented our own library for class creation. Utilizing the yaw position we were able to calculate setpoint and error for out PID controller, below is the logic followed to match angles on the robot frame and the real world. 

![BNO robot diagram](https://i.ibb.co/yQ2VbgM/bnodiagram.png)

Note that yaw is calculated using the Euler vector as shown in the equation below:

`var yaw = atan2(2.0*(q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);`

With this classes we are able to control our robot movement logically.
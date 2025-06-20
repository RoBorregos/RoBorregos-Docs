# Control 

To realize the control of our robot we rely on the advances of the 2024 team. So we reuse the motor libraries created previously:


>> 📁 motor

>> 📁 motors

## Kinematic

All of the Kinematic methods specifications can be found in the previous team documentation following this [link](/docs/SoccerOpen/2024/Control/index.md)

For this year, we had to develop a new form of control to adapt to our logic and use of vision. For that, the first thing we realized was a method able to normalize our IMU sensor angle so that our robot moves in a range of [-180,180], which also facilitates the correction path between the current_yaw and the setpoint.

![Image of the coordinate reference for Mirror camera](/docs/assets/soccer/Programming/Normalized_robot_angles.png)

```
double BNO055::analize_error(double setpoint, double current_yaw)
```

For the error calculated from the values retrieved from the mirror camera, we assign a PID through the `pid` object with which we determine the value of `speed_w`, which represents the angular correlation of the robot with respect to the center.

```
double error = bno.analize_error(setpoint, current_yaw);
double speed_w = pid.Calculate(setpoint, error);
```

On the striker Flor we have a front camera that like the mirror camera measures angles with respect to a centered reference. However, due to the angle and position of the camera, the center of reference is offset from the center of the robot.

![Image of the angle for front camera](/docs/assets/soccer/Programming/Front%20camara_normalized.png)

To solve this we designated a second PID object with the name `pid2` to adjust the motion of the robot based on these angles which are mathematically increased compared to the normalized angles. This generates a second angle correction variable called `speed_d`.

With these settings and classes we were able to move our robot more precisely with the frontal detection by making use of the double angular correction of our Striker robot.



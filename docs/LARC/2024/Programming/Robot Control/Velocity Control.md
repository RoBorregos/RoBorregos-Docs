## Holonomic Robot Overview

Encompassing mechanical movements aligned with the main engine's expectations, is overseen by the microcontroller. This entails control of four motors for movement, along with respective encoders, BNO055 IMU, floor phototransistors, elevator, and gripper. The control system accommodates mecanum wheel drive capabilities spanning forward/backward, left/right, and rotation.


## Encoder for feedback

 At the core of control lies the utilization of encoders to maintain uniform motor speeds, alongside continuous odometry updates for spatial consistency. The controllability of said wheel speeds depends on an implemented PID control loop for each wheel. Odometry is the driving force of this method and makes mecanum wheels viable. Given the myriad factors influencing chassis movement, such as weight distribution and mecanum wheel precision, IMU feedback supplement movement control, aiding directional precision.

## Kinematic Equations

As described below, these equations govern the direction and speed control of mecanum drive, facilitating precise movement across the field.

\[ \text{front_left} = \frac{1}{r}(v_x - v_y - (l_x + l_y)z) \]

\[ \text{front_right} = \frac{1}{r}(v_x + v_y + (l_x + l_y)z) \]

\[ \text{back_left} = \frac{1}{r}(v_x + v_y - (l_x + l_y)z) \]

\[ \text{back_right} = \frac{1}{r}(v_x - v_y + (l_x + l_y)z) \]

where

\[ v_x = \text{linear velocity in the X-axis} \]

\[ v_y = \text{linear velocity in the Y-axis} \]

\[ l_x = \text{Wheel base distance} \]

\[ l_y = \text{Wheel track distance} \]

\[ z = \text{Angular speed} \]




For autonomous driving, inverse kinematic equations are used. The equations are derived from mathematical modeling of a 4 Mecanum Wheeled Robot. These equations are used for calculating the required linear velocity for each individual motor at every instance of robot movement.


The previous equations generate linear velocity for each wheel, so that each one will be transforming linear velocity to angular movement. A linear velocity to RPM conversion is used. 

\[
\text{RPM} = \frac{\omega_i \cdot 60}{\text{wheelCircumference}}
\]


Later then, Output RPM serves as input feedback for the Embedded Control System. At the robot's core, PID type controllers for each motor’s function to maintain the robot’s stability over all its trajectory. Furthermore, phototransistors and both cameras bolster precision and facilitate orientation confirmation, alongside distance measurement to objects.



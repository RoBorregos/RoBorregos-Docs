## Explanation of the `orientedMovement` Function


The orientedMovement function integrates sensor data to adjust the robot's orientation by calculating and applying the necessary angular velocity. It ensures that the robot maintains or reaches the desired angle while allowing for linear movement in the X and Y directions. The use of proportional control helps in smooth and precise adjustments to the robot's orientation.

The `orientedMovement` function is designed to control a robot's movement, adjusting both its linear and angular velocities to maintain the correct orientation. Here's a step-by-step explanation of how the function works:

## Function Signature

    void Movement::orientedMovement(const double linear_x, const double linear_y, double angular_z) 

    Parameters:
        linear_x: Linear velocity along the X-axis.
        linear_y: Linear velocity along the Y-axis.
        angular_z: Angular velocity around the Z-axis.

## Updating Sensor Data



    Updates the orientation sensor (BNO055) and retrieves the current yaw angle of the robot. Then Normalizes the Current Angle



    if (current_angle < 0) {
    current_angle += 360;
    }

Ensures the current angle is within the range [0, 360] degrees.

## Calculating Angle Error



    angle_error = fmod(robotAngle_ - current_angle + 360, 360);
    if (angle_error > 180) {
    angle_error -= 360;
    }

    Computes the smallest angle difference between the desired angle `robotAngle_` and the current angle. The result, angle_error, is adjusted to fall within the range [-180, 180] degrees.

## Proportional Control for Angular Speed

A proportional term is defined to scale the angular speed based on the angle error.

    float proportional_term = 0.1; 

    if (fabs(angle_error) > kAngleTolerance_) {
    float angular_speed = fabs(angle_error) * proportional_term;
    angular_speed = constrain(angular_speed, -kMaxAngularZ, kMaxAngularZ);

    if (angle_error > 0) {
        angular_z = -angular_speed; // Rotate clockwise
    } else {
        angular_z = angular_speed; // Rotate counterclockwise
    }

    rpm = kinematics_.getRPM(0, 0, angular_z);
    } else {
    angular_z = 0.0;
    angleOffsetReached = true;
    rpm = kinematics_.getRPM(linear_x, linear_y, angular_z);
    }

Condition: Checks if the absolute angle error is greater than the defined tolerance `kAngleTolerance_`.

## If True:

1. Calculates the angular speed proportional to the angle error.

2. Constrains the angular speed to a maximum value `kMaxAngularZ`.

3. Determines the direction of rotation (clockwise or counterclockwise) based on the sign of the angle error.

4. Computes the RPM for each motor using the angular velocity.

## If False:

1. Sets the angular velocity to zero.

2. Marks that the angle offset has been reached.

3. Computes the RPM for each motor using the given linear velocities and zero angular velocity.


## Updating Motor Speeds


    updatePIDKinematics(rpm.motor1, rpm.motor2, rpm.motor3, rpm.motor4);

Sends the computed RPM values to the motors to adjust their speeds accordingly.



# Motion Control: Holonomic Movement

## Kinematic Model

The kinematic model was crucial for accurately calculating the speed of each motor, ensuring precise movement in the desired direction. The key principles applied include:

- **Motor Speed Calculation**: The speed of each motor was determined using kinematic equations based on the desired movement angle.
- **Consistent Orientation**: : Orientation data from sensors was used to correct the robot's movement, ensuring it always faced the goal.


## Sensors and PID Controller

We employed BNO-055 and MPU sensors to capture the robot's current orientation. A simplified PID (Proportional-Integral-Derivative) controller was implemented to correct any deviations from the desired orientation. This controller minimized the error between the current orientation and the target direction, facilitating smooth and accurate movement.

## Implementation

Here’s the core code that shows the kinematic equations and the corrections implemented using the PID controller:

```cpp
double PID::calculateError(int angle, int set_point) {
    unsigned long time = millis();
    double delta_time = (time - previous_time) / 1000.0;

    control_error = set_point - angle;
    double delta_error = (control_error - previous_error) / delta_time;
    sum_error += control_error * delta_time;

    sum_error = (sum_error > max_error) ? max_error : (sum_error < -max_error) ? -max_error : sum_error;

    double control = (kP * control_error) + (kI * sum_error) + (kD * delta_error);

    previous_error = control_error;
    previous_time = time;

    return control;
}

void Drive::linealMovementError(int degree, int speed, int error) {
  float m1 = sin(((60 - degree) * PI / 180));
  float m2 = sin(((180 - degree) * PI / 180));
  float m3 = sin(((300 - degree) * PI / 180)); 

  int speedA = (m1 * speed);
  int speedB = (m2 * speed);
  int speedC = (m3 * speed);

  speedA -= error;
  speedB -= error;
  speedC -= error; 

  motor_1.setSpeed(speedA); 
  motor_2.setSpeed(speedB); 
  motor_3.setSpeed(speedC); 
} 
```

### PID::calculateError
The calculateError function computes the control signal using the PID algorithm, considering the discrepancy between the desired and current orientation.

### Drive::linealMovementError
The linealMovementError function calculates the speed for each motor based on the desired movement direction and applies corrections using the error from the PID controller.


# Control overview

Robot control is the system in charge of moving (and controlling) the robot as expected.
This system is of relevance in Rescue Maze as it helps the robot move through obstacles and
locate itself, which is fundamental for exploring the whole maze using the algorithm.

## Sensors
One of the most important elements needed to develop a stable control is the use of reliable
sensors which give feedback to the robot about its current state. The sensor input is then 
used to move the robot accordingly and reach the target positions.

See [Sensors](./Sensors) for information about the sensors used.

## PID

>The PID controller is a control loop feedback mechanism widely used in industrial control systems.
A PID controller continuously calculates an error value as the difference between a desired setpoint
and a measured process variable and applies a correction based on proportional, integral, and derivative
terms (sometimes denoted P, I, and D respectively) which give their name to the controller type. 

- Copilot

In RescueMaze, a PID controller was used to make the robot move straight and rotate to the desired angles.
The PID control regulated the PWM signal sent to the motors such that they approached a target RPM.
In addition, the RPM targets were increased or diminished depending on the error between the current angle
and angle of the desired orientation.

See [PID](PID.md) for information about the PID implementation.


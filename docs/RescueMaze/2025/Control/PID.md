# PID

PID control was used to reach the requested RPMs for the motors. To reach the requested RPM, the behavior of the constants was plotted, in this way it was possible to observe how much it oscillated until reaching this, so after many modifications in these variables, it was possible to reach the desired behavior. The constants are kP, kI and kD, which indicate the relevance of the error in the proportional, integral and derivative.

This control has as reference the speed calculated with the ticks or interruptions of the encoders, for this it was checked that each motor had the same amount of ticks per revolution. 

## How does it work?

The encoder could be said to have an infrared that connects from side to side, so every time something passes through the middle of this "laser" there is an interruption. To get an approximation of how many ticks there are in a revolution you can make a code that counts the interrupts on the required pin with the arduino function attachInterrupt().

attachInterrupt() function:
[here](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)


In this way the speed of each motor is taken out so that the PID control allows to increase the pwm variable to reach the desired speed so that all motors go at the same speed.

## PID Orientation

To make the robot go in a specific orientation, the control was implemented with reference to the gyroscope (BNO), so that also for this one the constants had to be modified to reach as soon as possible its setpoint without much oscillation. For this the actual angle was used, i.e. the reference of the BNO and the desired angle if it was loaded more to one side the speed of the motors on the same side increased to reach their respective angle.

## Control speed respect target distance

The robot must move forward in a controlled manner, and sudden braking should be avoided so that it stops at the desired position. For this reason, as it approaches the target distance, its speed should gradually decrease. To achieve this, the map function is used by inputting the minimum and maximum speeds relative to the initial and final distances.

## PID Diagram

![PID_Diagram](/assets/maze/PID.png)


## PID Walls

In certain scenarios as would be the case of the ramp there are always walls on its sides so it was devised to apply a control with respect to the distances of the sides that is to say that if it was told to keep the same distance to both walls, this oscillated to reach that point and leave the ramp, of course the counters had to be modified to reach the desired behavior as well as the desired distance.


Impementation of PID:
[here](https://github.com/RoBorregos/RescueMaze2024/blob/nacionalNewMovement/PIDmotores/PID.cpp)
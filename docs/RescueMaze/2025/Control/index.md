# Control overview

Robot control is the system in charge of moving (and controlling) the robot as expected.
This system is of relevance in Rescue Maze as it helps the robot move through obstacles and
locate itself, which is fundamental for exploring the whole maze using the algorithm.

## Sensors
One of the most important elements needed to develop a stable control is the use of reliable
sensors which give feedback to the robot about its current state. The sensor input is then 
used to move the robot accordingly and reach the target positions.

See [Sensors](Sensors/index.md) for information about the sensors used.

## PID

>The PID controller is a control loop feedback mechanism widely used in industrial control systems.
A PID controller continuously calculates an error value as the difference between a desired setpoint
and a measured process variable and applies a correction based on proportional, integral, and derivative
terms (sometimes denoted P, I, and D respectively) which give their name to the controller type. 

In RescueMaze, a PID controller was used to make the robot move straight and rotate to the desired angles.
The PID control regulated the PWM signal sent to the motors such that they approached a target RPM.
In addition, the RPM targets were increased or diminished depending on the error between the current angle
and angle of the desired orientation.

See [PID](PID.md) for information about the PID implementation.

## Code

The main class of control is motors class and algorith only use this functions:
    To go forward
        -void ahead();

    To rotate   
        -void rotate(float);

    To do dispense kit and do the victim sequency with leds
        -void harmedVictim();
        -void stableVictim();
        -void unharmedVictim();

    To know if is wall in elect direction
        -bool isWall(uint8_t);

    To manage checkpoint
        -void checkpointElection();
    
    And global variables:
        -bool blackTile
        -bool blueTile
        -bool checkpoint
        -uint8_t victim
        -uint8_t kitState
        -bool buttonPressed
        -uint8_t rampState

All the rest of the functions and action are handled mainly for these function for exemple:
-read the TCS34725 and react if there is a black tile or wait if there is a blue tile
-read the limit switches and do a rutine
-read the encoder pulses and BNO055 angule to use PID adequately
-manage the ramps
-manage the vlx´s feedback

## Debug and calibration 
For debugging and calibrate, a OLED screen is used to show sensor detections, and a test class was made with functions to debug all the sensors and PID, aditional this class has calibrateColors function to detect and show rgb of the black, blue, default and checkpoint tiles

![ScreenCalibration](/assets/maze/screen.jpg)

## Problems

### Ramp

Because the Jetson Nano is located at the front, the robot may tip over at a certain speed. To address this, the variable rampCaution was implemented. It becomes true if the front VLX sensor detects a very large distance, causing the robot to move more slowly.
### Color detection 

An attempt was made to implement color detection using HSV values to avoid the need for calibration. However, the values were not consistent, so the system reverted to using calibrated RGB values.
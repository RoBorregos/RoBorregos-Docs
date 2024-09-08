## Electronics and Control
- Started development of PCBs for the omnidirectional mobile base.
- Standardized the electronics of the main robot for full 24V operation.

## Electronics and Control development
In order to achieve the full control of an omnidirectional base it was necessary to develop different PCBs and electronics to ensure that the base's movement, responses, and communication protocols met the standards and challenges set by the @Home competition

The most important part was the control implementation on the base's movement since it is intended to use a set of 4 omnidirectional wheels instead of mecanum wheels, which was the focus on the previous iteration. The initial step was to replace the old kinematics so the new wheel's movement could be supported. Also, a new PID was made so that the robot's sampling time for control purposes could be reduced up to 100ms with the aiming towards avoiding and filtering noise produced from the encoder's reading. Another important point related to the control was the consideration of the additional weight placed upong the robot's structure, so that the PID controller could move precisely the DC motors even in low speed.

Another important point about the new holonomic robot development was the need for more powerful motors and batteries, and the design of a new customized PCB for communication purposes and control. In the currnet state of the robot four 12v DC motors with 70Kg-Cm motors are used to move it, but it is planned to change them for more powerful brushless motors such as those used in FIRST competitions or even industrial-sized NEMA 23 or 34 motors with approximately 110kg-Cm or more torque. 

Finally, in order to ensure the controller PCB compatibility with high power motors and batteries, it is planned to change the drivers of the controller PCB, so they can controll 24v motors and high currents, such as those used in the NEMA motors.

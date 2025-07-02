# Motor, drivers and wheels

As seen in previous team documentation, our robots’ mobility system continued using 12-volt high-power Pololu motors, rated at 2200 revolutions per minute, which have proven to offer a strong balance of speed and torque for omnidirectional movement in RoboCup play. For the wheels, we relied on GTF Robot tires that had already demonstrated their reliability in past competitions and remained well-suited for the field conditions and demands of our league.

To drive these motors effectively, we implemented TB6612FNG MOSFET motor drivers on our custom PCB. Building on lessons learned from earlier designs, we paralleled the driver bridges to increase their current-handling capacity, ensuring that the system could deliver a continuous current of approximately 1.2 amps, while accommodating peak demands of up to 3 amps without risking damage. This configuration helped the robot maintain performance even during aggressive interactions with other robots.

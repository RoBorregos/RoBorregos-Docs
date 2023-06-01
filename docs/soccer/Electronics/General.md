# General 

## Design Software
For the PCB Design the EasyEda software was used.
The robot electronics are made up of 5 custom PCBs designed specifically for a specific use. The design of the PCBs was   carried out in the EasyEda software.

Initially, the design of the main PCB began. which has measurements of 10 x 10 cm.

For the main processing of our robot, an Arduino Mega Pro of 32 Bits and 2 Clocks of 16 MHz was used. It was mainly chosen for its light weight and its size reduction.


## Drivers 

HP Pololu 12V Motors at 2200 RPM were used. Our tires are GTF Robots that were in our laboratory from past competitions. For these motors we use Mosfet TB6612FNG type drivers. These drivers are bridged in parallel on the PCB, since HP motors demand a lot of current, especially when forcing between robots, giving us a current continuity of 1.2 and peaks of up to 3 Amps.

## How can we handle this motors with this drivers?

By bridging the motor outputs and input signals, the output channel can be increased up to 2.4 Amps continuous and up to 6 Amps peak. What allows us to use HP motors without danger of having damage to our PCB, drivers, or in the worst case, the motors themselves.

## Sensors
For the control of the robot a BNO 055 gyroscope sensor was used due to its reliability, performance and size. Ultrasonics were used for speed regulation with respect to the detected distance to prevent the robot from getting out of line.


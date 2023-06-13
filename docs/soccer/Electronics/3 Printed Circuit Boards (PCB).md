# PCB Designs


## IR Ring
Digital IR receivers: TSSP58038 were used to detect the IR signals emitted by the ball and a custom PCB was also designed.

The Ir ring is made up of 12 IR receivers, and an Atmega328p was used for processing and vectoring the infrared signals.



## Line Detection Boards
For the detection of the lines, independent PCBs were also designed for each of the three sides in the circumference of the robot.

These boards consist of 4 phototransistors each, getting an analog reading by processing our microcontroller on the main board.

The phototransistors we used were the TEPT5700. The reason we use these phototransistors is because of their gain, which allows the color reading to have a high difference range. In our case, when we detect white, the phototransistor gives a value close to 300 units. On the other hand, when it detects green, the value decreases to a value of 30, so we have a fairly reliable interval to distinguish between colors.

One setback with the choice of phototransistors was the color and incidence of light that had to be thrown at it. Initially the NJL7502L phototransistors were considered, but because these were in a deteriorated state they were discarded. Subsequently, we proceeded to search for those phototransistors that had a peak close to 600 nm, thus preventing them from detecting the infrared signal that is above 700 nm and causing us problems when detecting the lines.



## Main Board

The main board consists basically of an Arduino Mega Pro Shield for controlling the dedicated actuators and sensors for the robot to operate correctly.


Initially, the design of the main PCB began. which has measurements of 10 x 10 cm.

For the main processing of our robot, an Arduino Mega Pro of 32 Bits and 2 Clocks of 16 MHz was used. It was mainly chosen for its light weight and its size reduction.
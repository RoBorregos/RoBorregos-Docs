## Boards developed at the moment

- [Base controller board](https://oshwlab.com/joseacisnerosm1/homebase-atmega2560)

This board main goal is to control the motors of the base, which are 4 DC 12v/6v motors with 270:1 gear box reduction. Model [CQGB37Y001](http://www.cqrobot.wiki/index.php/Metal_DC_Geared_Motor_w/Encoder_CQGB37Y001), these motors come with magentic encoders already installed.
So the board has one [ATMega 2560](https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf) has the microcontroller, it has tow voltage supplies, one of 12v that can be conneceted thru a XT-60 connector or a terminal block.

![image info](/assets/home/HomeElectronicsControl/BaseConector.png)

And also has a 5v voltage supply provided by the serial port which also allows communication with the main computer. This port is supported by a female microsUSB that is conected to the FT232RQ chip. This chip also allows to use the a FTDI to acces the microcontroller.
![image info](/assets/home/HomeElectronicsControl/BaseCommunication.png)

Voltage can be interrupted manually by the headers shown bellow.

![image info](/assets/home/HomeElectronicsControl/BaseInterruptors.png)

For the drivers we have two Dual [MC33926](https://www.pololu.com/product/1213) Motor Driver Carrier, so each one controls two motors, giving us the total 4 motors we need. This driver was selected because its reliable protection features and its voltage-current range. 

For the orientation feedback we have the adafruit IMU sensor [BNO055](https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview).

![image info](/assets/home/HomeElectronicsControl/BaseIMU.png)


This board also has some miscelanous features such as extra voltage Outputs (5v/12V), I2C pins for communication, bootloader pins, and 6 extra digital pins for general purposes(emergency stop for example), indicator LEDs and a reset button.


- [SCARA controller and board](https://easyeda.com/a00831510/brazo_home_arduinomega)

This board is also based on the ATMega 2560 microcontroller but uses instead an Arduino Mega board plugged, this was donde like this because of components stock issues but it is going to be updated to have an SMD chip.
The board purpose is to controll all the other motors that are in the robot. Listing the motors we will have the next list.
Steppers
-Two [NEMA17](https://www.cytron.io/p-nema-17hs4401-bipolar-stepper-motor) steppers needed by the SCARA arm.
-One NEMA17 for the elevator
-And one NEMA17 for an extra degree of freedom for the gripper.

Giving us a total of four stepper motors plus 2 extra ones in case they are needed. So our board supports a total of six stepper motors, that are control by 6 drivers [TMC2208](https://www.trinamic.com/products/integrated-circuits/details/tmc2208-la/).
For the pinout to the drivers you can download the next [Arduino](stepperPruebas.ino) file or watch it directly in the PCB.

This board also supports feedback for the stepper motors, thru the [AS5600](https://pdf1.alldatasheet.com/datasheet-pdf/view/621657/AMSCO/AS5600.html) contactless potentiometer, which allows to know the position of the motor using a magnet fixed to the back of the motor. 

![image info](/assets/home/HomeElectronicsControl/NEMAS.png)


This model of encoder recquired the desgin of a housing so that the encoder and the motor will stay together. It uses communication via I2C, so for the correct use of this encoders it is also recquired to implement a multiplexor so that you can read multiple encoders. 

![image info](/assets/home/HomeElectronicsControl/Encoders.png)

Now for the servo motors, we mainly use two models, the [1245MG](https://www.pololu.com/file/0J706/HD-1235MG.pdf) when more torque is needed, or the smaller but less current demanding [MG995](https://pdf1.alldatasheet.com/datasheet-pdf/view/1132435/ETC2/MG995.html)
This board supports up to six 7v servomotors and the pinout to the motors is also declared on the Arduino file shown before.

And also has some miscelanous features such as extra voltage Outputs (5v/7v/12V), FTDI pins for communication, bootloader pins, indicator LEDS and voltage interruption headers.


- [Power board](https://oshwlab.com/a00830708/placaalimentacioncopia)

Lastly for the power supply board we just desgined a distribution circuit that can be interrupted via a Start and Stop buttons. The interruption is made by a two relay latch circuit.

![image info](/assets/home/HomeElectronicsControl/Powe1.png)

NOTE: The connection in the normally close and normally open  contacts is done like this because the blueprint and the real pinout is reversed. So in the real circuit, the connections go to the normally open contact.

Mainly in this board we just receive the main power supply and regulate it thru 3 [DC Buck converters](https://rees52.com/300w-dc-buck-converter-power-module-output-adjustable-20a-max-dc-dc-6-40v-step-down-to-12-36v-rs1831-2306-buck-converter/), each one for one of the operating voltages (5v, 7v or 12v). This board also has extra voltage outputs (5v/7v/12v).
 IMPORTANT: this board is in revision due to errors found during its implementation.
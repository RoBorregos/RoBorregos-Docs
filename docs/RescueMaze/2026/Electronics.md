# Electronics
Electronics consist of a system of two boards. The first is a power regulation board, which divides the 7.4V into three lines, a 6v for the motors, a 5v for the esp32, and a 3.3v for sensors. 
The second is a sensor board, which houses all the active components. A TCA9548A acts as an 8-channel I²C multiplexer, allowing the use of 7 VL53L0X which is a time-of-flight laser sensor for proximity and distance measurement. There is also a BNO055 which is a 9-degrees-of-freedom IMU that provides orientation data via accelerometer, gyroscope, and magnetometer; a VL53L0X, and a TCS color sensor for RGB light detection.



## Components:
- Esp32 devkit v1
- Raspberry pi 5
- Adafruit VL53L0X
- Multiplexor TCA9548A
- Driver tb6612fng
- Limit switch
- Adafruit BNO055
- Servomotor 
- mini560
- Pololu motors 6v
- LED strip


## Pcb 1
![](/assets/maze/Board1.jpeg)


## Pcb 2
![](/assets/maze/schematic_2.1.jpeg)
![](/assets/maze/Schematic_2.2.jpeg)
![](/assets/maze/Board2.1.jpeg)
![](/assets/maze/Board2.2.jpeg)



## Failures 
There were two boards made, the first one didn´t worked because of some tracks of the board had estrange angles which made it have a lot of noise. And the BNO went to a multiplexer channel, which it can't happen according to the datasheet. The second one had a LED on a strapping pin that made the esp32 reset.


## Areas of improvement:
- Change the microcontroller to another that has more pins or add a GPIO expander.
- Check there is not a component on the strapping pins.
- Try to make things smd to have more space.
- Put the VLX mounted on the board.
- Try to make the board as small and simple as possible.


## Recommendations:
- Ask for help and ensure the board is checked as many times as possible.
- Check that you have spare parts for everything.
- Make a calendar and try to follow it as much as possible.
- Communicate with all your team members so you are on the same page.
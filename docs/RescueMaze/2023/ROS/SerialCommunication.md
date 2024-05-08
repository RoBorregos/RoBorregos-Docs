# Serial Communication

Serial communication is used to communicate with the Arduino. The Arduino is connected to the Jetson Nano via USB. The communication is handled as a master-slave relationship, where the Jetson Nano is the master and the Arduino is the slave. The Arduino is programmed to listen to the Jetson Nano and respond accordingly. 

## Initial Communication Implementation

Initially, the communication was implemented using the [rosserial](http://wiki.ros.org/rosserial) library. The library is used to create a node that is used to send and receive data from the Arduino, allowing the use of ROS topics and services. However, due to problems with the library, the implementation was changed to use the python [serial](https://pyserial.readthedocs.io/en/latest/pyserial.html) library.

## Jetson Nano Implementation

The Jetson Nano uses the [serial](https://pyserial.readthedocs.io/en/latest/pyserial.html) library to communicate with the Arduino. The library is used to create a serial object that is used to send and receive data. 

The code implementation uses a set of functions that use specific commands to communicate with the Arduino. Each function uses a specific action number and a number of bytes to expect from the Jetson Nano. The Arduino is programmed to listen to the action number and respond accordingly. These functions include sending and receiving sensor data, as well as sending commands to the Arduino. 
This implementation also has a specific process to handle the sending and receiving of data, adding overhead to the packets sent and received. 
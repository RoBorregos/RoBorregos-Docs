# IR Ring
## General functionality
As previously mentioned, for our IR Ring, we use fifteen TSSP4038 IR sensors placed on a circular form in order to retrieve the approximate angle of the ball. Our method allows us to get an approximate angle error of around $\pm5^{\circ}$, this is more than enough to work with effectively with the IR ball. The error gets smaller the closer the robot is to ball, and the error only increases up to that range if the robot is not moving due to drift. It is important to mention that drift only occurs in cyclic intervals, and if the robot is not moving for extended periods of time, the drift will automatically fixed itself with SMA.

## Retrieving angle method explained
We measure the width under the pulse wave, from there, we compute the sine and cosine components. Getting our averages with the following model:

$$M_A=\frac{180}{\pi}\operatorname{atan2}\left(\frac{1}{k}\sum_{i=1}^k\sin\left(\theta_i\right), \frac{1}{k}\sum_{i=1}^k\cos\left(\theta_i\right)\right)$$

From this singular operation, we are able to get the approximate angle on where the ball may possibly be, however, if we only did this once, we will soon realize that the error explodes. Therefore, in order to finalize the method, you need to repeat this process for each datapoint $n$ number of times. In our case, this value is equal to $80$, and the error is minimal. 

> [!WARNING]
> Setting the $n$ value way to high may cause for data not to be sent on time, therefore returning nothing into the serial monitor.

## Communication with Teensy
Communication happens via UART; communicating from pins 0 and 1 (RX, TX) on the Arduino, and on pins 14 and 15 (RX, TX) on the Teensy. This communcation can only happen if and only if Arduino is sending data via `Serial` and the Teensy is receiving data from `Serial3`. 

Reading from Teensy example:
```cpp
#include <Arduino.h>
// Teensy 4.1 Hardware Serial 3 (Pin 15 = RX3, Pin 14 = TX3)
#define HWSERIAL Serial3 
void setup() {
    Serial.begin(115200);   
    HWSERIAL.begin(9600); 
}

void loop() {
    // Read string from Arduino Uno via UART and print to PC
    if (HWSERIAL.available() > 0) {
        String incomingData = HWSERIAL.readStringUntil('\n'); 
        incomingData.trim(); // Remove lingering \r carriage returns
    
    if (incomingData.length() > 0) {
        Serial.print("UART received Theta: ");
        Serial.println(incomingData);
        }
    }

  // Echo anything typed in the PC Serial Monitor back to the Uno (if needed)
    if (Serial.available() > 0) {
        String usbData = Serial.readStringUntil('\n');
        usbData.trim();
        
        HWSERIAL.println(usbData);
        Serial.println(usbData);
    }
}
```

Additionally, it is important that in order for the Teensy to be able to communicate with the Arduino, you need to use a level shifter, otherwise, if the Arduino tries to communicate with the Teensy directly; due to voltage differences, the Teensy will literally toast itself.

> [!WARNING]
> While it's true you can use 3.3V from the arduino's port, sometimes arduino decides to send more voltage than needed, which could potentially damage the teensy and straight up make it malfunction.

## Code upload to IR
In order to make changes to the code, you have two options:

I.- Use an FTDI to upload code directly into the microcontroller
II.- Use an auxiliary arduino to upload the code from that arduino, remove the microcontroller, and place the microcontroller into the PCB.

If you are using FTDI, consider the following:
1.- Use Arduino IDE as sometimes platformio environment fails to upload automatically with the FTDI.
2.- You need to have RX and TX disconnected from the other microcontroller.
3.- Verify connections from the FTDI before uploading. 
4.- Upload it as an arduino UNO.

If it failed, it can be for multiple reasons:
1.- If you get an access denied error, please close all serial monitor instances from arduino IDE. Check whether the solution was fixed. If its still denying you access, open device manager, go to ports, and re-install the port where the FTDI is homed to. After this, disconnect the FTDI from the port and connect it into another port.

2.- If you have a timeout error, this signifies that either your connections are incorrect (RX is connected with RX, or TX is connected with TX) or that the FTDI is fried. Remember that TX goes connected with RX, and RX goes connected with TX.

3.- If you are in the uploading process, arduino attempts to upload it, but constantly fails to the point that it starts making attempts. It means that either you bridge to connect the 5V and VIN malfunctioned, or you accidentally have it connected to the Teensy RX and TX ports, or the jumpers to which you are uploading your code are malfunctioning, or the FTDI is fried.
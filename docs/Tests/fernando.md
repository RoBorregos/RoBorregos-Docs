#Control documentation-MAZE_2025

##sensors
1. VLX
   vlx is a good sensor
2. BNO
   bno is a bad sensor

[Repository](
  https://github.com/RoBorregos/robocup-rescuemaze-2025
)

![TMR](https://github.com/RoBorregos.png)

##Code
```
void motors::setupMotors(){
    for(uint8_t i=0;i<4;i++){
        motor[i].initialize(Pins::digitalOne[i],Pins::digitalTwo[i],Pins::pwmPin[i],i);
        Serial.println(Pins::pwmPin[i]);
        myPID[i].changeConstants(kP_mov,kI_mov,kD_mov,movTime);
    }
    rampUpPID.changeConstants(kP_RampUp,kI_RampUp,kD_RampUp,rampTime);
    rampDownPID.changeConstants(kP_RampDown,kI_RampDown,kD_RampDown,rampTime);
    Wire.begin();
    screenBegin();
    screenPrint("r");
    bno.setupBNO();
    setupVlx(vlxID::frontLeft);
    setupVlx(vlxID::left);
    setupVlx(vlxID::front);
    setupVlx(vlxID::frontRight);
    setupVlx(vlxID::right);
    setupVlx(vlxID::back);
    setupTCS();
    leds.setupLeds();
    limitSwitch_[LimitSwitchID::kLeft].initLimitSwitch(Pins::limitSwitchPins[LimitSwitchID::kLeft]);
    limitSwitch_[LimitSwitchID::kRight].initLimitSwitch(Pins::limitSwitchPins[LimitSwitchID::kRight]);
    pinMode(Pins::checkpointPin, INPUT_PULLDOWN);
    servo.attach(Pins::servoPin);
    servo.write(servoPos);
    targetAngle=0;
}
```

##table
|hola|como|estas|
|----|----|-----|


> quote

---


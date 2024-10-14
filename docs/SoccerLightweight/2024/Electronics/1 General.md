# General

For the  TMR 2024, these are some general ideas:

 For the power supply we used two LiPO batteries: one 11.1V and 2250 mAh for the motors, and two 3.7V Lipo batteries connected in series to obtain 7.4V and 2500 mAh for the logic using a voltage regulator at 5V, These batteries were used due to their high amperage and optimal duration in competition. The PCBs, designed with EasyEda, included a main board with the microcontroller and sensors, boards for the TEPT5700 phototransistors connected directly to the microcontroller for line detection, and an IR ring with TSSP58038 receivers and an Atmega328p was reused to process the infrared signals. The Arduino Mega Pro microcontroller was chosen for its ease of use and multiple pins. For movement, HP motors at 2200 RPM with TB6612FNG controllers were used, configured to handle up to 2.4 Amperes continuous and peaks of 6 Amperes, adapting to the high current demand of the motors.

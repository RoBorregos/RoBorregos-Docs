# Communication 

We only used Teensy 4.1 as a microcontroller, so we took advantage of the UART Ports for serial communication. Our pipeline for the Striker Flor follows this structure:

![Pipeline for Striker Flor communication](/docs/assets/soccer/Programming/OpenMV_Teensy_diagram.png)

Since the Goalkeeper Capullo only contained one OpenMV H7, the pipeline simplifies to the following structure:

![Pipeline for Goalkeeper Flow communication](/docs/assets/soccer/Programming/OpenMV_single_Teensy.png)

Observe that the OpenMV H7 (mirror) sends a data package containing the following variables to Teensy 4.1

```
distance_b
angle_ball 
distance_g 
angle_goal 
distance_gop 
angle_gop
```

Othersides, the front OpenMV H7 (front) sends a smaller package focused only on ball detection

```
distance_b
angle_ball 
```

Othersides, the front OpenMV H7 (front) sends a smaller package focused only on ball detection

Note that OpenMV H7 (front) utilizes the UART port corresponding to Serial1, so OpenMV H7 (mirror) was assigned to function through Serial2.  

To process the whole data sent by the OpenMV H7 (mirror) the Teensy 4.1 follows these steps to transform the buffer array to changing variables. 

- Read characters from *Serial2* until a newline *\n* is found
- Store the received characters in a string *serial2_line*
- When a full line is received, call *processSerial2(line)*
- In *processSerial2()*:
    - Parse six float values: `ball_distance`, `ball_angle`, `goal_angle`, `own_distance`, `own_angle`
    - Assign values to their respective variable
    - Set flags:
        - `open_ball_seen` if ball distance and angle are non-zero
        - `goal_seen` if goal distance or angle is non-zero

To process the smaller data buffer from OpenMV H7 (front) the Teensy 4.1 follows this similar steps.

- Read characters from *Serial1* until a newline *\n* is found
- Store the received characters in a string *serial1_line*
- When a full line is received, call *processSerial1(line)*
- In *processSerial1()*:
    - Parse two float values from the line: `ball_distance` and  `ball_angle`
    - Assign values to `dribbler_distance` and `dribbler_angle`
    - Set flags:
        - `dribbler_ball_seen` if distance or angle is non-zero
        - `ball_captured` if distance ≤ 30 and angle is 0
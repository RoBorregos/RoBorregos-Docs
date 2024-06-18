For the algorithm design, which is the main logic, we utilized two main files. One for the esp32 and another one for the Raspberry Pi Pico. Our logic hierarchy follows this structure: 

>> Goalkeeper

>>  - ESP32_Goalkeeper

>>  - Pico_Goalkeeper


>> Striker

>> - ESP32_Striker

>> - Pico_Striker

### Goalkeeper Logic

The diagram below shows the logic flow for Goalkeeper in the Raspberry Pi Pico: 

![Goalkeeper Pico logic](\assets\soccer\Programming\goalkeeper_pico_diagram.png)

For the logic to center the robot in goal observe the pathway below: 

1. Check if the ball is found:

    - If the ball is found (`ball_found` is true), proceed to the next step.

2. Determine robot movement based on ball angle:

    - If the ball's angle is within -15 to 15 degrees (indicating the ball is almost directly ahead), move the robot forward towards the ball using a specific speed (`speed_t_ball`) and rotation speed (`speed_w`).
    - If the ball's angle is outside this range, adjust the ball angle to ensure it's within a 0-360 degree range and calculate a "differential" based on the ball angle. This differential is used to adjust the ball angle to a "`ponderated_angle`," which accounts for the ball's position relative to the robot. The robot then moves in the direction of this adjusted angle with the same speed and rotation speed as before.

3. Adjust robot position if the ball is not found but the goal angle is known:

    - If the goal angle is positive and the ball is not found:
        - If the goal angle is less than a certain threshold to the left, move the robot to the left (angle 270 degrees) to align with the goal using a specific speed (`speed_t_goal`) and rotation speed (`speed_w`).
        - If the goal angle is more than a certain threshold to the right, move the robot to the right (angle 90 degrees) to align with the goal using the same speeds.
        - If the goal angle is within the threshold, stop the robot's lateral movement but continue rotating at the current speed (`speed_w`).

### Striker Logic

The diagram below shows the logic flow for Striker in the Raspberry Pi Pico: 

![Striker Pico logic](\assets\soccer\Programming\striker_pico_diagram.png)

For the logic to implement if the ball was found observe the pathway below:

1. Check for Ball Detection or Distance Conditions:

    - The robot checks if the ball is found, if the distance to the ball is greater than 100 units, or if the distance is exactly 0 units. If any of these conditions are true, it proceeds with further checks; otherwise, it moves to the else block.

2. Log Ball Found:

    - If the condition is true, it logs "pelota found" to the Serial monitor, indicating that the ball has been detected or the distance conditions are met.

3. Direct Approach or Adjust Angle:

    - Direct Approach: If the ball's angle from the robot (considering a 180-degree field of view) is between -15 and 15 degrees, it means the ball is almost directly in front of the robot. The robot then moves directly towards the ball. The movement command uses an angle of 0 degrees, the absolute value of a predefined speed towards the ball (`speed_t_ball`), and a rotation speed (`speed_w`).
    - Adjust Angle: If the ball's angle is outside the -15 to 15-degree range, the robot needs to adjust its angle to approach the ball correctly. It calculates a "differential" based on the ball's angle (after adjusting the angle to a 0-360 range) and a factor of 0.09. This differential is used to calculate a "`ponderated_angle`," which adjusts the robot's movement direction either by subtracting or adding this differential, depending on the ball's angle relative to 180 degrees. The robot then moves in this adjusted direction with the same speed and rotation speed.

4. Default Movement:

    - If none of the conditions for the ball being found or the specific distance conditions are met, the robot executes a default movement. It turns around (180 degrees) with a speed of 170 units and the predefined rotation speed (`speed_w`), then pauses for 110 milliseconds. 
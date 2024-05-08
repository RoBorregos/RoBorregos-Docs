# Line Detection
To obtain the phototransistor values from the multiplexors, a function was created to obtain each value according to its binary code. In addition, we realized that each sensor would have different ranges when detecting green and white, so it was necessary to first make a calibration, which was different for the attacking and defending robot.

## Attacking Robot
Since this robot would go in all directions to search for the ball and score, it should never cross any white lines. Therefore, the phototransistors pcbs were used to estimate the angle on which the robot was touching the line to then move in the opposite direction. This was also complemented with ultrasonic sensors in order to avoid crashing with the walls, other robots and the ball itself (avoiding scoring on our goal).

<img src= "/assets/soccer/Programming/Attacking.png" alt= "Attacker detection" width="50%" height="50%">

The calibration for this robot was automatic and done when the robot started. Here, the robot would capture about 100 values for each sensor when standing on green to get the average measurement. Then, to check if there was a line, the robot would compare the current value with the average value and if the difference was greater than a threshold, then a line was detected and it was possible to see which phototransistor had seen it.

## Defending Robot
The defending robot worked basically as a line follower, using two phototransistors (one from each side) in order to move horizontally. Additionally, it used the front phototransistor pcb to check if it had gone too far back, hence it would move forward. There were also three ultrasonic sensors used to avoid crashing with the walls and the ball.

<img src= "/assets/soccer/Programming/Defending.png" alt= "Defending detection" width="50%" height="50%">

For this robot, it was important to check the two phototransistors for line following before the match started, since the robot would move proportionally according to the sensors, looking to stay in the average value between white and green. Therefore, the calibration was done manually, by reading and then setting the white and green values for each sensor. The front sensor was calibrated automatically using the method explained in the attacking robot.
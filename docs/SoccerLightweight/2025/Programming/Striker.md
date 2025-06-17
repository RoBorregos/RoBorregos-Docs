# Strategy
## Jamaica 🍹
The striker’s main objective is to locate the ball and attack. Unlike the goalkeeper, the striker does not operate within a fixed area. Instead, it constantly checks that it is not crossing the field boundaries using phototransistor plates placed underneath the robot. If a boundary line is detected, the robot adjusts its position to stay within the playing field.

The striker follows a state machine to manage its behavior. Initially, it enters a search state, using its IR ring to detect the ball. Once it locates the ball, it evaluates the angle between its forward direction and the ball; if this angle is 10° or less, the robot assumes it has possession.

Upon gaining possession, it transitions to an attack state. It uses a camera to determine the angle to the opponent’s goal and adjusts its movement accordingly, heading straight toward the goal. To ensure a valid score, the striker is programmed to stop just before entering the goal area, using the front phototransistors to detect the boundary line and avoid crossing it entirely.
# Strategy

## Horchata 🥛

Our defending robot was programmed to operate within a defined zone on the field. This area was limited both horizontally and vertically to ensure it stayed close to the goal and maintained an effective defensive position.

To define the vertical boundaries, the robot used an ultrasonic sensor that measured its distance from the back wall of the field. Based on this reading, it was restricted to move only between a minimum and maximum distance—ensuring it didn’t get too close to the goal or drift too far forward.

For the horizontal boundaries, we used data from the camera. A custom function allowed the robot to determine its angle relative to the goal. Using this information, it was limited to move only within a certain angular range, preventing it from straying too far left or right.

Together, these constraints formed a kind of virtual rectangle or box within which the robot could move. Inside this area, it used its IR ring to follow the infrared signals emitted by the ball, allowing it to track and react to the game while staying in position. This strategy gave the goalkeeper both spatial awareness and the ability to contribute intelligently to the match without overstepping its defensive role.

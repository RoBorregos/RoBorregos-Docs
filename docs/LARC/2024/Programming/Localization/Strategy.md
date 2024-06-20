## Color Pattern Detection

Since a way to detect the color objects is available, the robot just needed to detect the color squares and compare the sequence. For this, it is important to have a lot of filters, since the color cubes have almost the same color. So in order to prevent false patron detection, the strategy was to implement only reading the upper colors, and the condition was that they were within a margin in the “x” and “y” directions of the other colors of the sequences. Doing this, it was possible to detect the most possible “x” position of our robot, since it is only necessary to check the sequence and the most centered color. 

## Overview of Strategy

Equipped with this information, the robot asks for Camera System information to send feedback at first instance when initial position is set.
The autonomous navigation system follows a sequence of operations to efficiently locate, identify, and manipulate colored cubes within a designated environment. The process begins with the robot rotating until it localizes a distinct color pattern, saving the current angle as an absolute offset for reference. Subsequently, it calculates the “x” coordinates using a color pattern detector and determines which square to move towards based on proximity to the origin. For instance, if the robot is at X: 0, it moves forward until detecting the specified color corresponding to the “x” coordinate (e.g., green). Key coordinates such as 0, 3, and 6 guide the robot's movement strategy, ensuring optimal navigation within the environment.


Upon detecting the target color, the robot drives towards it until the TCS (color sensor) confirms the presence of the color. A specific routine is then executed to move backward, ensuring the robot remains within the boundaries of the squares. Following this, the robot rotates 180 degrees and searches for cubes while recording the last known global position before detecting a color. Depending on the detected color and the robot's position, precise movements are executed to align the robot with the cube's position for grasping.
Once aligned, the robot adjusts its linear velocity based on the error in image with the “x” coordinates and moves forward until the bounding box indicates the cube is directly in front of it. The color of the cube is then detected, and a sequence of actions make the robot grab and release the cube at the appropriate coordinate based on its color. Throughout these maneuvers, the Nema motor (stepper motor) controls the gripper's position, ensuring accurate manipulation of the cubes.
After releasing the cube, the robot rotates towards the shelves to continue its mission, iterating through the process from step 6 to locate and handle additional cubes as required.


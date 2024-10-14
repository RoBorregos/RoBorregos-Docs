# Navigation
### Dashgo B1 mobile base

#### Multi-level obstacle avoidance
- Developed a node for fusing LIDAR and on-arm depth camera for better obstacle avoidance.

#### Person following 
For this task, the robot has to find the person to follow, then calculate their position, and finally send it to the navigation server periodically to follow them.

First, the robot uses the depth camera to find the person. The depth camera is mounted on the arm of the robot, so the robot has to move the arm to a specific position to be able to search for the person. 

The robot then employs the Mediapipe pose landmarker model to find the person's landmarks, which are given as pixel coordinates. The pixel coordinates are then converted to 3D coordinates using the depth camera. The robot then transforms the 3D coordinates to the base frame and sends the position to the navigation server.

The navigation system utilizes the Hector SLAM package for mapping and localization in unknown environments, as well as obstacle avoidance. Previously the robot used the AMCL package for localization, but it wasn't able to localize the robot accurately in unknown environments. 

One problem found with the Hector SLAM package is that sometimes the footsteps of the person are detected as obstacles, so the robot stops following the person. A temporary solution was to decrease the speed of the robot to give it more time to mark the footsteps as free cells. Another proposed solution is to use the depth camera to detect the person's footsteps and mark them as free cells manually in the occupancy grid. This solution is still under development.

### Omnidirectional Mobile Base
- Started development of the omnidirectional mobile base, with a custom ROS node for control and odometry.
- Integrated Hector SLAM for mapping and localization.
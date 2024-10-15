# Achievements from 2023

This year was focused around expanding and updating the software areas of our current hardware setup, focusing in the tasks for the competition, and a side new project based on the development of a custom omnidirectional mobile base.
After participating in the Mexican Robotics Tournament, a lot of senior college member of the team graduated, transfering the knowledge and leadership. New features were built on top of the previous developments, with the purpose of submitting the qualification material for the RoboCup 2024 competition.

Here is our uploaded [Team Description Paper](https://athome.robocup.org/wp-content/uploads/OPL-RoBorregosTDP2023.pdf) and [Video](https://www.youtube.com/watch?v=rPmSxZlzO78), for the Second Call for Participation for the Open Platform League.

## [Electronics and Control](Electronics%20and%20Control/index.md)
- Started development of PCBs for the omnidirectional mobile base.
- Standardized the electronics of the main robot for full 24V operation.

## [Mechanics](Mechanics/index.md)
- Started development of the omnidirectional mobile base, with an octagonal shape and 4 omnidirectional wheels.
- Designed and started manufacturing of several new grippers.
- Redesigned the electronics protections for the Dashgo and Xarm6.

## [Human Robot Interaction](Human%20Robot%20Interaction/index.md)
### Speech Recognition
- Developed a ROS node for speech recognition using Whisper, replacing our online solution from Azure.

### Action Interpretation
- Developed custom fine tuned GPT-3.5 model for action interpretation, with emphasis on a more flexible and human-like interpretation of voice commands.

### Robot Interface
- Developed a ROS Node for continuous arm movement to follow the user, enhancing the robot's interaction capabilities.
- Installed and started development on an on-robot tablet for user interaction, including RVIZ and camera visualization and a custom interface for the robot's services.

## [Integration and Networks](Integration%20and%20Networks/index.md)
### ROS
- Replaced a previous web socket solution for communicating two roscores with a multimaster solution, allowing for more robust and faster communication between the robot software modules.

## [Computer Vision](Computer%20Vision/index.md)
### Human Recognition
- Replaced DeepFace for face_recognition from dlib, allowing for faster and more accurate face recognition.
- Developed a custom human attribute recognition using the PETA dataset.
  
### Object Detection
- Migrated to Yolov8 for object detection.
- Increased adaptability and ease of use of the automated dataset generation method.
- Started development of a custom dataset generation tool based on 3D scanning.

## [Navigation](Navigation/index.md)
### Dashgo B1 mobile base
- Developed a node for fusing LIDAR and on-arm depth camera for better obstacle avoidance.
- Started work on a person follower node integrating SLAM for navigation on unknown environments.

### Omnidirectional Mobile Base
- Started development of the omnidirectional mobile base, with a custom ROS node for control and odometry.
- Integrated Hector SLAM for mapping and localization.

## [Manipulation](Manipulation/index.md)
### Planning and Hardware
- Replaced Xarm5 for a Xarm6, as well as designed new custom grippers.
- Migrated the planning and arm control computation to the Jetson AGX Xavier, for stable compute times and a more reliable ethernet-based communication with the arm.
- Started a migration from MoveIt planning to faster alternatives.

### Pick and Place
- Increased accuracy and stability of the pick process.
- Developed two place methods, one with a graph based approach and another using clustering for identifying possible placing areas.

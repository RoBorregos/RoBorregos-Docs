# Developments from 2022-2023

## Electronics and Control
Designed and manufactured a PCB for handling the SCARA robot's motors and arm. 

## Mechanics
Integrated the Xarm5 robot arm with the Dashgo B1 mobile base, as well as developed a custom gripper and electronics protections.

## Human Robot Interaction
### Speech Recognition
Developed a ROS node for speech recognition using Azure's Speech Service.

### Action Interpretation
Developed a ROS node for action interpretation using custom embeddings and a GPT-3 model.

## Integration and Networks
### ROS
Developed the main engines for the competition tasks, including General Purpose Service Robot, Receptionist and Carry my Luggage.

### Computers
Integrated a Jetson AGX Xavier as the robot's main computer as well as a Jetson Nano for the mobile base.

### Networking
Integrated the on-robot computers, Xarm and base within a wired network, as well as a wireless router for full internet access and for external computing resources.

## Computer Vision

### Pose Estimation
Developed a pose estimation ROS node, mainly used for detecting simple actions.

### Object Detection
Switched to YOLOv5 for object detection and developed a method for fast and [semi authomatic dataset generation](Object%20Detection/Dataset%20Automatization.md).

### Human Analysis
Used DeepFace for face recognition and analysis as well as object detection models for clothing description.
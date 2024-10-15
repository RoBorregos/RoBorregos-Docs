# Face following

To allow for a more human-like interaction, the robot was programmed to follow a person's face when it recieves new instructions. This was achieved by using the face [detection and recognition node](../../Computer%20Vision/Human%20Recognition/Face%20detection.md), which publishes the position of the largest face detected in the frame. This way, joints from the arm are adjusted to keep the person's face centered in the camera's field of view.

<p align="center">
<img src= "/assets/home/HRI/FaceDet.png" alt= "Face following" width="80%" height="80%">
</p>

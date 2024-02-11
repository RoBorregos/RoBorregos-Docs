# Face following

To allow for a more human-like interaction, the robot was programmed to follow a person's face when it recieves new instructions. This was achieved by using the face detection and recognition node, which publishes the position of the largest face detected in the frame. This way, joints from the arm are adjusted to keep the person's face centered in the camera's field of view.
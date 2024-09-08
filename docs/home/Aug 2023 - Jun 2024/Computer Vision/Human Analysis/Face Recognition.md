# Face detection and recognition

In order to improve efficiency of face detection and recognition, several models were tested, given that past implementations of `DeepFace` were unstable. Finally, `face_recognition` from dlib was selected as it allowed for faster and more accurate results.

## Face detection
Before the detection process, previous recognized faces are first loaded from a folder with known people images named with a unique id, thus obtaining and adding each face encoding to a list of known people. Next, each frame from the ZED is processed, finding the face locations and encodings of each face in the frame. In addition, in order to reduce the comparisons during the recognition phase, faces are tracked according to their position and a determined threshold. This way faces that remain in almost the same position as the previous frame are not processed again.

## Face recognition
For the recognition process, each encoding and location from the frame is compared to the known faces array to determine if there is a match and obtain the corresponding id. If there are no matches, then the face is loaded to the folder of known people and assinged a new id, which is also stored in a json file, which also contains basic characteristics of a person.

## ROS integration
The detection and recognition node is subscribed to the ZED camera topic, and currently publishes two topics: 
- Detection results: which contains the bounding boxes and IDs of the detected faces as a custom list message.
- Move topic: which contains the position of the largest detected face in the frame, allowing the robot to follow the person.
# Human Robot Interaction
HRI aims to create intuitive and natural ways for humans to interact with robots, allowing for seamless integration of robots into various domains such as healthcare, manufacturing, entertainment, and personal assistance.

In Roborregos, our work in this field has been divided into three different goals:

### [Human physical analysis](Human%20Physical%20Analysis/Face%20Following.md)
-	Replaced DeepFace for face_recognition from DLib, allowing for faster and more accurate face recognition.
-	Developed a custom human attribute recognition using the PETA dataset.

### [Speech](Speech/Action%20Interpretation.md)
It is divided into 2 subsections:

  **1.	[Speech Recognition](Speech/Human%20Speech%20Processing.md)**
- Developed a ROS node for speech recognition using Whisper, replacing our online solution from Azure.
  
**2. [Action Interpretation (NLP, natural language processing)](Speech/Action%20Interpretation.md)**
- Developed custom fine tuned GPT-3.5 model for action interpretation, with emphasis on a more flexible and human-like interpretation of voice commands. 

### [Robot Interface](Robot%20Interface/Display.md)
- Developed a ROS Node for continuous arm movement to follow the user, enhancing the robot's interaction capabilities.
- Installed and started development on an on-robot tablet for user interaction, including RVIZ and camera visualization and a custom interface for the robot's services.

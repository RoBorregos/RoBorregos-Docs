# Text To Speech


Consists of 1 component that is a ROS node with topics.

1. Say
    
    **devices/say [python]**: It is a node that say through the speakers what is published under _robot_text_ topic. It has a topic to notify another nodes that the robot is talking  _inputAudioActive_. It uses Google [gTTS engine](https://gtts.readthedocs.io/en/latest/) as an online alternative or [pyttsx3](https://pyttsx3.readthedocs.io/en/latest/engine.html) as an offline alternative.

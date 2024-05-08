# Vision

Computer vision is an important area in Rescue Maze as it allows to complete some of the tasks that provide the most points.
In Maze, vision was used to detect visual and colored victims, which give feedback to the robot of when should rescue kits be dropped.

The cameras used in this competition were connected to the Jetson Nano to process the output and perform the necessary tasks. In particular, 2 openmv cameras were used, one for each side of the robot. The cameras were connected to the Jetson Nano via USB and used Serial communication.

Both cameras runned a RPC script, which allowed the Jetson Nano to send commands to the cameras and receive data from them. 2 main calls were performed: one to get the image from the camera (to process in Jetson) and another to get the main detected color (processed in the camera).

## Sections

- [OpenMV](Openmv.md): More information of how to use the OpenMVs and RPC.
- [TensorFlow Lite](TensorFlowLite.md): More information of how to train models and use them for image inference.


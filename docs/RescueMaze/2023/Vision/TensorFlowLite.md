# TensorFlow Lite

>TensorFlow Lite is a framework that allows to run machine learning models on embedded devices. It is a lightweight version of TensorFlow, which is a framework for machine learning developed by Google. TensorFlow Lite is used in Rescue Maze to run machine learning models on the Jetson Nano, which is the embedded device used in the robot.

\- Copilot

There are 3 main steps to use TensorFlow Lite in Rescue Maze:

1. Generate the dataset
2. Train a model
3. Run the model on the Jetson Nano

## Generate the dataset

See a dataset example [here](https://github.com/RoBorregos/rescuemaze-2023/tree/pidrotation/maze_ws/src/devices/openmv_camera/scripts/datasets/dataset1). The dataset is composed of images taken with the OpenMV camera. The images are divided in folders, each corresponding to a specific class. The images of each class should take into account the varitions of how the object can be seen in the camera. For example, if the object is a victim, the images should be taken from different angles, distances, and lighting conditions. The scripts used to generate the datasets were: [dataset_capture_script.py](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/maze_ws/src/devices/openmv_camera/scripts/datasets/dataset_capture_script.py) (runned in the OpenMV), and [save_image.py](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/maze_ws/src/devices/openmv_camera/scripts/datasets/save_image.py) (runned in external computer, where images were saved).

## Train a model

- Here is an [end-to-end example](https://www.tensorflow.org/lite/models/modify/model_maker/image_classification) of process to train a new model.
- Here is the script used to train this year's models: [image_classifier_maze.ipynb](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/maze_ws/src/devices/openmv_camera/scripts/datasets/image_classifier_maze.ipynb)

## Run the model on the Jetson Nano

- Here is the script used to run the models on the Jetson Nano: [camera_controller.py](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/maze_ws/src/devices/openmv_camera/scripts/camera_controller.py)

Once the model is trained, the script will ouput a .tflite file. This file is loaded in **camera_controller.py** to make inferences. The model returns an array of probabilities, each corresponding to a class.
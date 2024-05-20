# Shelf Object detection

For the Storing Groceries task, objects need to be stored in shelves, where each level has items of the same category. Therefore it was necessary to first identify the objects that are in each shelf and group them by their level for HRI to determine the category of each level.

## Detection

To detect the objects, the `YOLO v8` model was used (80 classes), combined with the `YOLO v5` (360 objects) model to detect several objects from the shelf. This way, an array of detections was obtained containing the bounding box, class, name and score of each object.

## Clustering

To group the objects by level, a `KMeans` clustering algorithm from [sklearn](https://scikit-learn.org/stable/) was used. This way the objects were cluster according to the min y-coordinate of the bounding box, since all objects of the same level should be on similar starting y-coordinates, while using the center y-coordinate could vary if the objects had different sizes.

## Areas of improvement

Combining several models worked to detect several objects. However, some classes would sometimes overlap and the results could have the same object twice. Therefore, it would be ideal to use the model trained with the objects from the competition.

## ROS Node

The node begins the clustering process as soon as the service is called, recieving a boolean value and returning a message of type shelf which contains an array of shelf_levels, each level containing a height value (average of heights of that level), a label and the objects detected in that level (string array).

The ROS node is structured in the following way:

### Subscriber topics

- `/zed2/zed_node/left/image_rect_color`: ZED camera topic
- `/zed2/zed_node/depth/depth_registered`: ZED depth topic
- `/zed2/zed_node/depth/camera_info`: ZED camera info topic

### Publisher topics

- `/vision/3D_shelf_detection`: Publishes markers for 3D visualization in RVIZ.
- `/vision/img_shelf_detection`: Publishes an annotated image for debugging and display purposes.

### Service topics

- `/vision/shelf_detector`: Of type `ShelfDetections`, recieving bool and returning a message of type shelf.



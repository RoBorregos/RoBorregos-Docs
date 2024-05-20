# Person Counting and Finding

For the GPSR task, it was necessary to count and identify people who meet certain characteristics. Thus, a node for this services was developed.

## Person Counting

## Person Identifying

## Node structure

For the counting process, the node has two services, one to begin counting and one to end the process and return the count. On the other hand, the finding service is called once and it publishes the coordinates of the person identified.

The ROS node is structured in the following way:

### Subscriber topics

- `/zed2/zed_node/left/image_rect_color`: ZED camera topic

### Publisher topics

- `/vision/person_detection`: Publishes a Point with the x and y coordinates of the tracked person. (Does not publish if the track person is not visible)
- `/vision/img_tracking`: Publishes an annotated image for debugging and display purposes.

### Service topics

- `/vision/change_person_tracker_state`: Of type `SetBool` to enable or disable tracking.

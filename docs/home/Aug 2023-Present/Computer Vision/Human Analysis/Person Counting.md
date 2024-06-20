# Person Counting and Finding

For the GPSR task, it was necessary to count and/or identify people who meet certain characteristics. Thus, a node for this services was developed.

## Person Counting
For the person counting module, `YOLOv8` was used to detect people and `Mediapipe` was used to detect the different poses of each person. Implementing the `REID` module, each person was counted once, keeping a vector with the differnt poses to return only the final count of a single pose.

## Person Identifying
A similar process was followed, however, once a pose is identified, the coordinates of the person are published.

## Node structure

For the counting process, the node has two services, one to begin counting and one to end the process and return the count. On the other hand, the finding service is called once and it publishes the coordinates of the person identified.


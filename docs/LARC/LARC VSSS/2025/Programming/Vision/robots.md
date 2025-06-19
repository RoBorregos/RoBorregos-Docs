# Robot detection

The robot detection system combines computer vision techniques with machine learning to accurately identify and track robots on the field. The process begins with a YOLO-based detection model that locates each robot by recognizing its unique visual patterns. Once detected, the system calculates both the robot's position and orientation, which are essential for navigation and strategy execution.

''  Insert code here ''

## Position 

For position estimation, the center point of each detected bounding box is transformed from image coordinates to real-world field coordinates using a homography matrix. This transformation accounts for perspective distortion and ensures accurate positioning relative to the field dimensions.

''  Insert code here ''

## Orientation

Orientation determination relies on detecting colored markers placed on each robot. Using HSV color segmentation, the system isolates the colored region within the robot's bounding box and calculates its centroid. The orientation angle is then derived from the vector connecting the robot's center point to this colored marker centroid. This method provides a reliable way to determine which direction the robot is facing.

''  Insert code here ''

## Reading's stability

To improve stability and reduce noise, the system implements a moving average filter for both position and orientation data. This smoothing technique helps maintain consistent tracking even with minor detection variations. Finally, the processed coordinates and orientation angles are transmitted via UDP to the control system, enabling real-time decision making and coordinated movement during matches.

''  Insert code here ''
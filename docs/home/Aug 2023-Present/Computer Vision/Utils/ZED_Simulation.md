# ZED_Simulation

Node to simulate ZED topic, useful for testing and debugging without the need of the camera.

```python
#!/usr/bin/env python3
import rospy

import time
from sensor_msgs.msg import Image
import cv2
from cv_bridge import CvBridge

class ZedSimulation():
    
    def __init__(self):
        rospy.init_node('zed_simulation')
        self.bridge = CvBridge()
        self.image_pub = rospy.Publisher("/zed2/zed_node/rgb/image_rect_color", Image, queue_size=10)

    def run(self):
        while not rospy.is_shutdown():
            cap = cv2.VideoCapture(0)

            while cap.isOpened():
                ret, frame = cap.read()

                if not ret:
                    break
                
                ros_image = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')
                self.image_pub.publish(ros_image)
                cv2.imshow('frame', frame)
                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break
                
            cap.release()
            cv2.destroyAllWindows()
p = ZedSimulation()
p.run()
```
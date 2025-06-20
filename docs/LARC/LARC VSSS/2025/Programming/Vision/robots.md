# Robot detection

The robot detection system combines computer vision techniques with machine learning to accurately identify and track robots on the field. The process begins with a YOLO-based detection model that locates each robot by recognizing its unique visual patterns. Once detected, the system calculates both the robot's position and orientation, which are essential for navigation and strategy execution.

## Position 

For position estimation, the center point of each detected bounding box is transformed from image coordinates to real-world field coordinates using a homography matrix. This transformation accounts for perspective distortion and ensures accurate positioning relative to the field dimensions.

```python
    for res in results:
        boxes = res.boxes  

        for box in boxes:
            x1, y1, x2, y2 = box.xyxy[0]
            x_center = float((x1 + x2) / 2)
            y_center = float((y1 + y2) / 2)

            robot_coors = np.array([[x_center, y_center]], dtype="float32")
            robot_coors = np.array([robot_coors])
            real_robot_coors = cv2.perspectiveTransform(robot_coors, H)[0][0]
```

## Orientation

Orientation determination relies on detecting colored markers placed on each robot. Using HSV color segmentation, the system isolates the colored region within the robot's bounding box and calculates its centroid. The orientation angle is then derived from the vector connecting the robot's center point to this colored marker centroid. This method provides a reliable way to determine which direction the robot is facing.

```python
def get_color_centroid(img):
    """
    Detects the centroid of the largest colored region in the image.
    
    Args:
        img (ndarray): Input image.
    
    Returns:
        tuple: Centroid coordinates (x, y) and the detected color.
    """
    imgBlur = cv2.GaussianBlur(img, (7,7), 0)
    hsv_img = cv2.cvtColor(imgBlur, cv2.COLOR_BGR2HSV)                                          

    max_area = 0
    centroid = None
    biggest_color = None
    for color, ranges in hsvRanges.items():
        lower = np.array(ranges['lower'])
        upper = np.array(ranges['upper'])

        mask = cv2.inRange(hsv_img, lower, upper)

        kernel = np.ones((5, 5), np.uint8)

        dilatedMask = cv2.dilate(mask, kernel, iterations=1)
        erotedMask = cv2.erode(dilatedMask, kernel, iterations=1)

        contours, _ = cv2.findContours(erotedMask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        if contours:
            largest_cnt = max(contours, key=cv2.contourArea)
            area = cv2.contourArea(largest_cnt)
            if area > max_area:
                cv2.imshow("Mask", mask)
                max_area = area
                M = cv2.moments(largest_cnt)
                if M['m00'] > 0:
                    cX = int(M["m10"] / M['m00']) 
                    cY = int(M['m01'] / M['m00']) 
                    centroid = (cX, cY)
                    biggest_color = color
    
    return centroid, biggest_color

def get_orientation(img, color_centroid):
    """
    Calculates the orientation of the robot based on its centroid and a color marker.
    
    Args:
        img (ndarray): Region of interest containing the robot.
        color_centroid (tuple): Centroid of the color marker.
    
    Returns:
        float: Orientation in radians.
    """
    bb_shape = img.shape
    if color_centroid != None:
        cX = float(bb_shape[0] / 2)
        cY = float(bb_shape[1] / 2)
        robot_centroid = (cX, cY)

        dx = robot_centroid[0] - color_centroid[0]
        dy = robot_centroid[1] - color_centroid[1]

        #return orientation in degrees; change depending on control needs
        orientation = (math.atan2(dy, dx)) #use math.degrees() % 360 for degrees use
        cv2.line(img, (int(color_centroid[0]), int(color_centroid[1])), (int(robot_centroid[0]), int(robot_centroid[1])), (0,255, 0), 2)
        return orientation
    else:
        return None
```

## Reading's stability

To improve stability and reduce noise, the system implements a moving average filter for both position and orientation data. This smoothing technique helps maintain consistent tracking even with minor detection variations. Finally, the processed coordinates and orientation angles are transmitted via UDP to the control system, enabling real-time decision making and coordinated movement during matches.

```python
def moving_average_centroid(self, centroid):
    """
    Calculates the moving average of centroids for smoothing.
    
    Args:
        centroid (tuple): Current centroid (x, y).
    
    Returns:
        tuple: Smoothed centroid (x, y).
    """
    if centroid is not None:
        self.centroid_history.append(centroid)
        if len(self.centroid_history) > 0:
            avg_x = sum([c[0] for c in self.centroid_history]) / len(self.centroid_history)
            avg_y = sum([c[1] for c in self.centroid_history]) / len(self.centroid_history)
            return (avg_x, avg_y)
    return centroid

def moving_average_orientation(self, new_orientation=None):
    """
    Calculates the moving average of orientations for smoothing.
    
    Args:
        new_orientation (float): Current orientation in radians.
    
    Returns:
        float: Smoothed orientation in radians.
    """
    if new_orientation is not None:
        self.orientation_history.append(new_orientation)
    if len(self.orientation_history) > 0:
        return sum(self.orientation_history) / len(self.orientation_history)
    return self.orientation
```
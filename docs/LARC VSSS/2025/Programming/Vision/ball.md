# Ball detection

## HSV segmentation

The ball is detected using HSV color segmentation, where the input image is converted to HSV space for robust color filtering. A predefined range (colorParams) isolates the ball's color, generating a binary mask. Noise is reduced through morphological operations (dilation + erosion), and the largest valid contour is identified as the ball.

## Polynomial Extrapolation

To predict the ball's movement, a moving average window stores recent positions. A 2nd-degree polynomial fit extrapolates the next position, improving tracking during fast motion. This helps compensate for processing delays, ensuring smoother coordinate updates.
 
The next code shows how HSV identification is then improved with polynomial extrapolation

```python
def findObject(image, copy, H): 
    """
    Detects the object in the image and calculates its real-world coordinates.
    
    Args:
        image (ndarray): Input image for object detection.
        copy (ndarray): Copy of the image for visualization.
        H (ndarray): Homography matrix for perspective transformation.
    
    Returns:
        tuple: Real-world coordinates of the detected object.
    """
    global ball_positions
    imgBlur = cv2.GaussianBlur(image, (7,7), 0)
    imgHSV = cv2.cvtColor(imgBlur, cv2.COLOR_BGR2HSV)                            

    lower = np.array(colorParams[0:3]) 
    upper = np.array(colorParams[3:6])
    mask = cv2.inRange(imgHSV, lower, upper)#
    
    kernel = np.ones((5, 5), np.uint8)
    
    dilatedMask = cv2.dilate(mask, kernel, iterations=1)
    erotedMask = cv2.erode(dilatedMask, kernel, iterations=1)
    result = cv2.bitwise_and(image, image, mask=erotedMask)
    cv2.imshow("Better mask", erotedMask) 

    objCenter,objPts = findContoursAndSize(dilatedMask, copy) 
    if objCenter:
        cv2.circle(image, (int(objCenter[0]), int(objCenter[1])), 2, (255,0,0), 5)
        objCenterPt = np.array([objCenter[0], objCenter[1]], dtype="float32")
        print(f"Normal: {objCenterPt[0]}, {objCenterPt[1]}")

        ball_positions.append((objCenterPt[0], objCenterPt[1]))
        if len(ball_positions) > MOVING_AVG_WINDOW:
            ball_positions.pop(0)

        if len(ball_positions) >= MOVING_AVG_WINDOW:
            ball_positions_np = np.array(ball_positions)
            x_extrapolated = polynomial_extrapolation(ball_positions_np[:, 0])
            y_extrapolated = polynomial_extrapolation(ball_positions_np[:, 1])
            objCenterPt = (x_extrapolated, y_extrapolated)
        cv2.circle(image, (int(objCenterPt[0]), int(objCenterPt[1])), 2, (0, 255, 0), 5) 
        print(f"Extrapolado: {objCenterPt[0]}, {objCenterPt[1]}")

        realFldCoors = cv2.perspectiveTransform(np.array([[[objCenterPt[0], objCenterPt[1]]]], dtype="float32"), H)[0][0]
        cv2.putText(image, f"({realFldCoors[0]:.1f}, {realFldCoors[1]:.1f}) cm", (int(objCenter[0] + 50), int(objCenter[1] + 20)),cv2.FONT_HERSHEY_SCRIPT_SIMPLEX, 0.5, (255, 255, 255), 2)
        return (realFldCoors[0], realFldCoors[1]) 
    else:
        return (0, 0)
```

## Coordinate extrapolation

Detected ball positions are transformed from pixel coordinates to real-world field coordinates using a homography matrix, enabling accurate positioning for robot navigation.

```python
def getHomography(cap, realCoor):
    """
    Computes the homography matrix based on user-clicked points and real-world coordinates.
    
    Args:
        cap (cv2.VideoCapture): Video capture object for live feed.
        realCoor (list): List of real-world coordinates corresponding to the clicked points.
    
    Returns:
        ndarray: Homography matrix mapping pixel coordinates to real-world coordinates.
    """
    global clicked_points
    clicked_points = []

    cv2.namedWindow("Calibration")
    cv2.setMouseCallback("Calibration", mouse_callback)
    
    while len(clicked_points) < 4: 
        success, img = cap.read()
        if not success:
            continue
        cv2.putText(img, f"Click on Point {len(clicked_points) + 1} out of 4", (50, 50), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2)
        
        for pt in clicked_points:
            cv2.circle(img, pt, 3, (0, 0, 255), -1)
        cv2.imshow("Calibration", img)
        
        if cv2.waitKey(1) & 0xFF == ord("q"):
            raise Exception("Calibration aborted")
    
    cv2.destroyWindow("Calibration")

    pxCoors = np.array(clicked_points, dtype = "float32")
    realCoors = np.array(realCoor, dtype = "float32")

    H, _ = cv2.findHomography(pxCoors, realCoors, cv2.RANSAC, 5.0)
    
    return H
```

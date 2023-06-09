## Aruco

An ArUco marker is a synthetic square marker composed of a wide black border and an internal binary matrix that determines its identifier.

##Integration

ArUco detection was integrated using ArUco library from OpenCV, getting ArUco bounding boxes and aruco class, using the following code:

```python
import cv2
import cv2.aruco as aruco
from vision_utils import *
import numpy as np

dictionary = cv2.aruco.Dictionary_get(cv2.aruco.DICT_4X4_50)

parameters = cv2.aruco.DetectorParameters_create()

cap = cv2.VideoCapture(0)

while True:
    # Capture a frame from the camera
    ret, frame = cap.read()

    # Convert the frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect ArUco markers
    corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(gray, dictionary, parameters=parameters)

        # Print list of detected ArUco markers and their corners
    print(ids)

    if corners:
        for i, marker_corners in enumerate(corners):
            print(ids[i])
            corner = corners[0][0]
            xmayor = np.amax(corner[:, 0])
            ymayor = np.amax(corner[:, 1])
            xmenor = np.amin(corner[:, 0])
            ymenor = np.amin(corner[:, 1])

            print(f"Xmayor: {xmayor:.2f}, Xmenor: {xmenor:.2f}, Ymayor: {ymayor:.2f}, Ymenor: {ymenor:.2f}")    
    # Draw detected markers on frame
    frame_with_markers = cv2.aruco.drawDetectedMarkers(frame, corners, ids)

    # Show output frame
    cv2.imshow("Output", frame_with_markers)

    # Exit if 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()

```


![ArUco Detector using webcam](../../assets/LARC/arucos.jpg)
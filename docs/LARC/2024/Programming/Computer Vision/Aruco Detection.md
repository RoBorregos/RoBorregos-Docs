For this detection the OpenCV technology and libraries were used in order to make it easier. At first, the filters applied were gray to make it easier to detect. Then, a dictionary was made out of ArUco values. After that, using Opencv.arcu the ArUco marker was detected. Now the only thing that was left was to filter for non important markers. The same thing is done for Color detection, where a filter is used with a minimum area requirement. Afterwards, it is possible to filter for the specified values of the ArUco marker, since the values are already known in advance.

Main code used is written below

## Import OpenCv for coordinates localization and searching for regions of interest and other dependencies needed

    import cv2
    import numpy as np
    import pathlib
    import sys
    import Constants

    from vision_utils import *

    class DetectorAruco:
    def __init__(self):
        self.cx = 0
        self.cy = 0

        self.flag = False
        self.mask  = None
        self.cv_image = np.array([])

        self.boxes = []
        self.detections = []

        self.x_pixels = 0
        self.y_pixels = 0

        self.aruco_detections_data = None

        # Flags 
        self.first_iteration = True 
        self.img_flag = True

![Aruco Detection](docs/assets/LARC/arucos.png)

## Main function for detecting ArUcos

    def detectar_arucos(self, img):
        dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_250)
        parameters =  cv2.aruco.DetectorParameters()
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(gray, dictionary, parameters=parameters)
        frame_with_markers = cv2.aruco.drawDetectedMarkers(img, corners, ids)
        img = frame_with_markers

        bb = []
        detections = []
        tempo = []
        if ids is not None:
            if corners:
                for i, marker_corners in enumerate(corners):
                    corner = corners[i][0]
                    xmayor = np.amax(corner[:, 0])
                    ymayor = np.amax(corner[:, 1])
                    xmenor = np.amin(corner[:, 0])
                    ymenor = np.amin(corner[:, 1])
                    #get x and y centroid
                    self.cx = (xmayor + xmenor)/2 / img.shape[1]
                    self.cy = (ymayor + ymenor)/2 / img.shape[0]

                    # draw a point on the centroid
                    cv2.circle(img, (int(self.cx * img.shape[1]), int(self.cy * img.shape[0])), 5, (0, 0, 255), -1)
                    #print(f"Xmayor: {xmayor:.2f}, Xmenor: {xmenor:.2f}, Ymayor: {ymayor:.2f}, Ymenor: {ymenor:.2f}")    
                    tempo =  ymenor, xmenor, ymayor, xmayor
                    bb.append(tempo)

                    #Remove brackets from ids
                    tmp_d = str(ids[i]).strip('[]')
                    detections.append(tmp_d)
                    #ids[i][j] Es el id del aruco
                    #corners Es la bounding box del aruco
            self.get_objects(bb, detections)
        return img

    def get_objects(self, boxes, detections):
        res = []
        #sort boxes full content by first parameter, then second, and save in new obj
        sorted_boxes = []
        sorted_detections = []

        for box in boxes:
            appended = False
            for i in range(len(sorted_boxes)):
                if sorted_boxes[i][1]>box[1] and abs(sorted_boxes[i][0]-box[0])<50 or sorted_boxes[i][0]-box[0]>50:
                    sorted_boxes.insert(i, box)
                    sorted_detections.insert(i, detections[boxes.index(box)])
                    appended = True
                    break
            if not appended:
                sorted_boxes.append(box)
                sorted_detections.append(detections[boxes.index(box)])
        boxes = sorted_boxes
        detections = sorted_detections

        for index in range(len(boxes)):
            """
            y values
            """
            diferencey = abs(float(boxes[index][2])-float(boxes[index][0]))/2
            halfy = self.y_pixels/2
            yc = diferencey + float(boxes[index][0])
            midpointy = yc - halfy
            """ 
            x values
            """
            # [label, xmin, xmax, pixel en x]
            diference = abs(float(boxes[index][3]) - float(boxes[index][1]))/2
            half = self.x_pixels/2 
            xc = diference + float(boxes[index][1])
            midpoint =  xc - half# (xmax - xmin)/2 + xmin - width
            # print("min: " + str(boxes[index][1]) + " xc: " + str(xc) + "midpoint: " + str(midpoint) + " diference: " + str(diference) + " half " + str(half))
            res.append([str(detections[index]), float(boxes[index][1]), float(boxes[index][3]), float(boxes[index][0]), float(boxes[index][2]), midpoint, midpointy ])
        self.aruco_detections_data = res            
    
    def setUp(self,img):
        y, x, _= img.shape
        self.x_pixels = x
        self.y_pixels = y
    
    def main(self):
       #Runs camara
        camara_index = Constants.camara_index
        cap = cv2.VideoCapture(camara_index)
        while True: 
            ret, frame = cap.read()
            self.image = frame
            if self.first_iteration:
                y, x, _= frame.shape
                self.x_pixels = x
                self.y_pixels = y
                self.first_iteration = False
            
            if self.img_flag: 
                self.updated = frame
                self.cv_image = self.updated

            self.detectar_arucos()
            cv2.imshow("frame", self.updated)
            self.boxes = []
            self.detections = []
            self.img_flag = True
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

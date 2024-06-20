# Person Tracking

Person tracking requires the robot to identify a person and be able to follow them as they move around the environment. This process is required in several tasks, such as Carry My Luggage and GPSR (General Purpose Service Robot). Therefore, computational vision is required to detect, identify and track a person.

## Person detection

In order to detect people, the `YOLO v8` model was used, obtaining the bounding boxes of each person in each frame. Additionally the `ByteTracker` algorithm was implemented to automatically assign a track_id to each person that is still visible from the previous frame. However, using the default tracker from YOLO was not consistent enough, since people who go behind large objects or even other people are assigned new ids, since this tracker does not keep track of people who leave and re-enter the frame. This introduced the necessity of a re-identification model that could recognize people who have appeared in previous frames, specially because the tracked person could exit the frame at any moment and it is necessary to identify them again to continue following.

```python
# Get the results from the YOLOv8 model
results = self.model.track(frame, persist=True, tracker='bytetrack.yaml', classes=0, verbose=False)
```

## Person re-identification

To re-identify people, the repository [Person_reID_baseline_pytorch]("https://github.com/layumi/Person_reID_baseline_pytorch") was used as a base to train different models using the `Market1501` dataset. More specifically, the models `ResNet50`, `Swin`, `PCB` and `DenseNet` were trained and tested, eventually opting for the `DenseNet` model as it was the lightest. With the model trained, it was possible to obtain a feature vector from an image with a person.

```python
# Crop the image using the bbox coordinates
cropped_image = frame[y1:y2, x1:x2]

# Convert the image array to a PIL image
pil_image = PILImage.fromarray(cropped_image)

# Get feature
with torch.no_grad():
    new_feature = extract_feature_from_img(pil_image, self.model_reid)
```

Nonetheless, extracting the embeddings from each person in every frame is not efficient, therefore an array of previous detections was kept to check if there is a new person detected. In such case, the embeddings would be extracted and compared to the embeddings of the tracked person using a cosine difference threshold.

```python
def compare_images(features1, features2, threshold=0.6):
    if features1.ndim != 1 or features2.ndim != 1:
        return False

    # Compute cosine similarity between feature vectors
    similarity_score = 1 - cosine(features1, features2)

    # Compare similarity score with threshold
    if similarity_score >= threshold:
        return True  # Same person
    else:
        return False  # Different person
```

## Media pipe implementation

An issue encountered with the detection and re-identification process was that the YOLO model would sometimes identify parts of people as a person, meaning that bounding boxes would sometimes contain only hands or heads, for example. This became problematic as the re-identification model could return unexpected results for this cases. Therefore, the `MediaPipe` library was used to detect the chest of a person, and only then the re-identification model would be used.

```python
pose_model = mp.solutions.pose.Pose(min_detection_confidence=0.8)

def check_visibility(poseModel, image):
    pose = poseModel
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Process the image
    results = pose.process(image)

    # Check if the pose landmarks are detected
    if results.pose_landmarks is not None:

        # Get the x and y coordinates of the chest
        chest_x = results.pose_landmarks.landmark[11].x
        chest_y = results.pose_landmarks.landmark[11].y
        chest_visibility = results.pose_landmarks.landmark[11].visibility

        # Check if the chest is in the frame
        if (chest_x < 0 or chest_x > 1 or chest_y < 0 or chest_y > 1) and chest_visibility < 0.95:
            return False
        else:
            return True
```

## Node structure

In order to start tracking, the service should be called, recieving a boolean to start or end the process. Once the service is called to begin tracking, the person detected with the largest bounding box area is assigned as the tracked person. While the tracking process is still enabled, the node will publish the coordinates of the person relative to the image (in pixels) as long as the tracked person is in frame, else it will not publish anything.

The ROS node is structured in the following way:

### Subscriber topics

- `/zed2/zed_node/left/image_rect_color`: ZED camera topic

### Publisher topics

- `/vision/person_detection`: Publishes a Point with the x and y coordinates of the tracked person. (Does not publish if the track person is not visible)
- `/vision/img_tracking`: Publishes an annotated image for debugging and display purposes.

### Service topics

- `/vision/change_person_tracker_state`: Of type `SetBool` to enable or disable tracking.

### Launch file

For easier execution, the node can be launched using the following command:

```bash
roslaunch vision receptionist.launch
```

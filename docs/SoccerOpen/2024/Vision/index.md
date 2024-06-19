### Blob Detection

For detection we first had to set some constants using VAD values to detect the orange blob, we also added a brightness, saturation and contrast filter to make the image obtained from the camera more clear. 

Our vision algorithm followed these steps: 

1. Initialize sensor
2. Locate blob
3. Calculate distance
4. Calculate opposite distance
5. Calculate goal distance
6. Calculate angle
7. Main algorithm
8. Send data package to esp32 via UART

The first step when using the camera includes reducing the vision field by placing a black blob to reduce image noise, we also locate the center of the frame using constants such as `FRAME_HEIGHT`, `FRAME_WIDTH` and `FRAME_ROBOT`.

For the game we must be able to differenciate between 3 different types of blobs that include the following: 

- Yellow goal
- Blue goal
- Orange ball

We created the `locate_blob` method that used `find_blobs` from the OpenMV IDE to locate blobs in the image snapshot for each specific threshold value set and returns a list of blob objects for each set. In the case of the ball we have set the `area_threshold` to 1, this small value means that even when a small area of this color is detected it will be marked as an orange ball to increase the amount of inclusivity in our vision field. For the case of the goals the `area_threshold` is set to 1000 because goals are much more larger in size. 

Observe the image below to see the different blobs detected from the camera's POV.

![Robot vision view camera](\assets\soccer\Programming\robot_vision_view.jpg)

Once we have our different blobs detected we can calculate the distance to the blob using the hypotenuse. First we need to calculate the relative center in x and y axis, then find the magnitude distance and finally using an exponential regression model to calculate the real distance with pixel comparison. Note that the expression was obtained by running measurments comparing cm and pixels using a proportion and data modeling on Excel.

```
magnitude_distance = math.sqrt(relative_cx**2 + relative_cy**2)
total_distance = 11.83 * math.exp((0.0245) * magnitude_distance)
```

For the `goal_distance` we needed to calculate the opposite distance, using sine. 

```
distance_final = goal_distance*math.sin (math.radians(goal_angle))
```

To calculate the angle we used the inverse tangent and then just convert to degree measurment. 

```
angle = math.atan2(relative_cy, relative_cx)
angle_degrees = math.degrees(angle)
```

Finally, depending on the blob that is being detecting for each image snapshot we perform the corresponding methods and sent the data package using a format of two floating points divided by commas. 

### Line Detection

To avoid corrsing the lines located outside the goals we implemented the `pixel_distance` measurment, so when our robot reached certain distance it automatically moved backward so we can limit its movement to avoid crossing the white lines. 

See the image below to observe the line limitations from the camera's POV. 

![Line robot view camera](\assets\soccer\Programming\line_robot_view.jpg)
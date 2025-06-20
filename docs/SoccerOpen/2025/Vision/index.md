# Vision

The vision of the robot was a crucial part of the development and performance of our robots. Fot this it is important to clarify that most of the constants and parameters must be adjusted before every match mostly because of the light variations. 

## Blob detection 

The first step for the detection of the objects is to use the OpenMV IDE to detect the blobs of our objects. For this we make use of adjustable LAB values that should be close to what we are looking for for a given object. In our case, we had to detect an orange ball, a blue goal and a yellow goal. Also, for different scenarios we modified the values of exposure, brightness, contrast and saturation.

To do so, we used the tool of threshold editor offered by OpenMVIDE. 

OpenMVIDE > Tools >  Machine Vision > Threshold editor

## Algorithm for OpenMV H7 (Mirror) 

1. Initialize sensor and variables (Camera, resolution, gain, etc)
2. Locate blob (`find_ball(img)`, `find_goal(img)`, `find_goal_opp(img)`)
3. Calculate ball and goal distances (`distance_ball(blob)`, `distance_goal(blob)`)
4. Calculate angles (`-(angle(blob) - 180.0)`)
5. Main algorithm 
6. Send package to Teensy 4.1 via UART 

```
data = "{} {} {} {} {} {}\n".format(distance_b, angle_ball, distance_g, angle_goal, distance_gop, angle_gop)
uart.write(data)
pyb.delay(50)
```

## Algorithm for OpenMV H7 (Front) 

1. Initialize sensor and variables (Camera, resolution, gain, etc)
2. Locate blob (`find_ball(img)`)
3. Calculate ball distance (`distance_ball(blob)`)
4. Calculate angle (`-(angle(blob) - 180.0)`)
5. Area restriction 

```
if area < 510:
distance_b = 0
angle_ball = 0
```
6. Main algorithm 
7. Send package to Teensy 4.1 via UART 

```
data = "{} {} {} {} {} {}\n".format(distance_b, angle_ball)
uart.write(data)
pyb.delay(50)
```

The first step when using the mirror camera is to set the reference center of our robot by modifying `X_CENTER` and `Y_CENTER`; this happens to change constantly because of robot impacts and quick fixes. 

Secondly, to detect the blobs for our three objects we created methods that captures blobs existing in each frame creating sets of blobs that can be merged for each object, allowing us a better personalization (`find_ball(img)`, `find_goal(img)`, `find_goal_opp(img)`). This allowed us to change significant data to differ our objects from image noise. In our case we established de `area_threshold` of the ball to be 20 pixels for large distances, and for the goals we established an area of 200 consequence of the illumination and the standard size of goals.

Visualize how the camera detects the object in the field by encapsulating each in a different color for identification.

![Detecting ball and goals using mirror](/docs/assets/soccer/Programming/OpenMV_vision.jpg)

For the angles and distances calculation, based on last year's team, we also decided to approximate the calculations using a coordinate system. taking into account that we set our center in the middle of the robot and mirror, we can calculate the distance and angles relative to the robot using trigonometry. Basically, we calculate the relative distances for the center of the object in the x and y axis. With this information we can calculate the relative distances in pixels, calculating the hypotenuse, and the angle using the `atan2()` function in micropython. 

```
#Distance calculation 
relative_distx = blob.cx() - X_CENTER
relative_disty = blob.cy() - Y_CENTER

vector_dist = math.sqrt(relative_distx**2 + relative_disty**2)
distance = vector_dist

#Angle calculation
angle = math.atan2(relative_disty, relative_distx)
angle_degree = math.degrees(angle)
```

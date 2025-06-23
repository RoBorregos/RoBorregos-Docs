# Vision

For the vision part of the robot we use a Pixy 2.1, an intelligent camera that detects objects by color. We mainly use it to identify the goal, which can be yellow or blue depending on the side we are playing on. The Pixy communicates with our microcontroller via SPI, which allowed us to have fast and reliable transmission of the data we needed in real time.

What the Pixy did was to detect objects that matched the color signatures that we previously configured. It would send us information such as:

- The signature (which told us if it was a yellow or blue goal),

- The x and y position of the center of the object,

- The width and height of the object.

With the x-coordinate, we calculated the angle between the robot and the goal. For that we used a formula based on the Pixy's field of view (which is 60°) and its horizontal resolution (320 pixels):
```cpp
float PixyCam::getGoalAngle(int x){
  float angle = (x-160)*(60.0/320.0)*-1;
  angle_values[angle_index] = angle;
  angle_index = (angle_index + 1) % kMovingAverageSize; // Incrementar el índice circularmente

  // Calcular el promedio móvil
  float sum = 0;
  for (int i = 0; i < kMovingAverageSize; i++) {
      sum += angle_values[i];
  }
  return sum / kMovingAverageSize;
}
```

This gave us an angle value representing which way the goal was with respect to the front of the robot. We multiplied this by -1 to adjust the direction of the angle according to our convention. In addition, we applied a moving average to smooth the value and prevent the robot from reacting unsteadily if the detection was not as accurate.

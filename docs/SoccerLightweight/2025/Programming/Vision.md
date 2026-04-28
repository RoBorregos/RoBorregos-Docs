# Vision 

For the vision part of the robot we use a Pixy 2.1, an intelligent camera that detects objects by color. We mainly use it to identify the goal, which can be yellow or blue depending on the side we are playing on. The Pixy communicates with our microcontroller via SPI, which allowed us to have fast and reliable transmission of the data we needed in real time.

What the Pixy did was to detect objects that matched the color signatures that we previously configured. It would send us information such as:

- The signature (which told us if it was a yellow or blue goal),

- The x and y position of the center of the object,

- The width and height of the object.

### Obtaining the data
To have a more organized way of retrieving different data from the camera:
1. Signature
2. Number of Objects
3. x coordinate
4. y coordenate
5. width
6. height
7. angle 
   
We created a `struct` so that we can access each value in the same way:

```cpp
struct TargetGoalData{
	uint8_t signature;
	int x;
	int y;
	int width;
	int height;
	float cameraAngle;
};
```

We used a `for` loop to read the data for all the objects detected by the camera with the same signature.  
Then, a `condition` was used to ensure we were retrieving data from the signature of interest (opponent's goal for the striker and our goal for the goalkeeper):

```cpp
for (int i = 0; i < numberObjects; i++){
  uint8_t signature = getSignature();
  if (signature == targetSignature){
    int x = getX(i);
    int y = getY(i);
    int width = getWidth(i);
    int height = getHeight(i);
    float cameraAngle = getGoalAngle(x);
    return {signature, x, y, width, height, cameraAngle};
  }
}
```
Finally, we can call each value using the same structure in the `main.cpp` file.

```cpp
TargetGoalData targetGoalData = pixy.getTargetGoalData(numberObjects, targerSignature);
    Serial.print(targetGoalData.signature);
    Serial.print(targetGoalData.x);
    Serial.print(targetGoalData.y);
    Serial.print(targetGoalData.width);
    Serial.print(targetGoalData.height);
    Serial.println(targetGoalData.cameraAngle);
```
### Camera angle
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

This gave us an angle value representing which way the center of the goal was with respect to the front of the robot. We multiplied this by -1 to adjust the direction of the angle according to our convention. In addition, we applied a moving average to smooth the value and prevent the robot from reacting unsteadily if the detection was not as accurate.

## For future development
We recommend developing a simpler code to test how the robot reacts to the information obtained from the camera methods before integrating it into the main functions.
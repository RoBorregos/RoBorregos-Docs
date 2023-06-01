# Vision

## Target detection
For goal detections, an Open MV H7 camera was used. Using the Open MV IDE, blob color detection was possible using micropyhton scripts. With this, the bounding box was identified and sent to the arduino. This measures were then used by both robots when going towards the goal or estimating the distance to the goal.

## UART communication
When sending information to the arduino, we faced several issues as the program would sometimes get stuck. We eventually realized that this was due to the protocol that we were using, as the buffer would sometimes receive an incomplete message and get an error when trying to process it. Therefore, to solve this issue, we changed the way to send and receive messages. In python the message was sent in the following format:

```python
uart.write(f"{tag},{x},{y},{w},{h}\n")
```

Here, the tag value was either an *a* or a *b* according to the color of the goal, then the x and y values were the center of the blob and w and h the width and heigth. this message was then received on the arduino on the Serial 3 port, using the following code:

```cpp
void updateGoals() {

  if (Serial3.available()) {
    String input1 =  Serial3.readStringUntil('\n');

    if (input1[0] == 'a')
      yellowGoal.update(input1);
    else if (input1[0] == 'b')
      blueGoal.update(input1);
  }

}
```
For this first function, we received the bounding box and if the message began with an *a*, then the object yellow goal was updated and the same thing occured if it began with a *b*. It was important to first identify when the message began, since sometimes due to the buffer space, messages could be cut and begin with numbers or commas. 

Then, for the object update, the following code was used:

```cpp
void Goal::update(String str) {
  int arr[4];
  String data = "";
  int index = 0;

  for (int i = 2; i < str.length() && index < 4; i++) {

    if (!(str[i] == ',')) {
      data += str[i];
    } else if (str[i] == ',' || i == str.length() - 1) {
      arr[index++] = data.toInt();
      data = "";
    }

    x = arr[0];
    y = arr[1];
    w = arr[2];
    h = data.toInt();

    area = w * h;

  }

}
```
In this function, it was important for the loop to run until either the length of the string was done or the index got to three, which meant that 4 values were read. It was important to keep this counter, since once again, due to the buffer size, sometimes messages would be cut and combined with other values, which resulted in a longer string with more commas that would eventually cause an error.

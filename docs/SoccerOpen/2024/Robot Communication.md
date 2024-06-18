Taking into account that we used a dual microcontroller we utilized various channels for serial communication. Our pipeline follows this structure:  

![Serial Communication](https://i.ibb.co/2yD3PXs/serialdiagram.png)

Note that the first data package input occurrs in the camera, sending the following variables to the Raspberry Pi Pico: 

```
filtered_angle
ball_distance
ball_angle
goal_angle
distance_pixels
```

In our case the Raspberry Pi Pico just works to format into String and prepare our data package in a way so the esp32 can interpret it and work with it. 

Once the data has reached the esp32 the data management follows these steps: 

1. Reading data from serial input
2. Initializing an array and an index
3. Tokenizing the string
4. Converting tokens to integer and storing in array
5. Repeat the process until the 5 values have been populated
6. Assigning parsed values to variables
# Vision System: Goal Detection

In our Soccer Lightweight project, detecting the goal is essential for strategic gameplay. We employed a Pixy2 camera, which facilitated blob color detection using the PixyMon IDE. This system enabled our robots to identify the bounding box of the goal and transmit the relevant data to the Arduino for processing.

## Pixy2 Camera and PixyMon IDE

The Pixy2 camera, integrated with the Open MV IDE, allowed for the detection of colored blobs representing the goals. The bounding box coordinates of these detected blobs were then transmitted to the Arduino, enabling the robots to navigate towards the goal or estimate the distance to it.

## Implementation

Here's the core code that shows how the vision system updates goal data and checks for detected goals:

```cpp
void Goals::updateData() {
    pixy.ccc.getBlocks();
    numGoals = pixy.ccc.numBlocks > 2 ? 2 : pixy.ccc.numBlocks;
    for (uint8_t i = 0; i < numGoals; i++) {
        goals[i].x = pixy.ccc.blocks[i].m_x;
        goals[i].y = pixy.ccc.blocks[i].m_y;
        goals[i].width = pixy.ccc.blocks[i].m_width;
        goals[i].height = pixy.ccc.blocks[i].m_height;
        goals[i].color = pixy.ccc.blocks[i].m_signature;
    }
}

bool Goals::detected(uint8_t color) {
    for (uint8_t i = 0; i < numGoals; i++) {
        if (goals[i].color == color) {
            return true;
        }
    }
    return false;
}
```

### updateData()
This function retrieves the detected blocks from the Pixy2 camera and updates the goal data by storing the coordinates, dimensions, and color signatures of up to two detected goals.

### detected()
This function checks if a goal of the specified color has been detected. It iterates through the detected goals and returns true if a match is found.




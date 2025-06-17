# Maze Exploration Algorithm

It is based in a Depth exploration using DFS as an algorithm with Dijkstra as path planning
algorithm. This implementation avoids blue and black tiles while keeping record of the
victims already detected.

## Implementation

First, the algorithm was based on a basic DFS exploration implemented with recursion and
back movements. However, when leading with embedding systems, memory and execution
time should be the most optimal. Therefore, recursion was not an option and using
dynamic memory was not available. So, an iterative DFS algorithm was implemented along
with a linked list performed as a Stack. Moreover, to optimize back-movements, a reverse
movement was applied to optimize the robot’s positions, but to optimize the exploration it
should implement a path planner and the perfect algorithm was Dijkstra. Finally, Tiles were
optimized, data structures were stablished at a custom size due to the lack of memory in
the ESP32 microcontroller and implemented a map saving function.

## DFS Exploration

This was implemented with a clockwise priority, focusing on always exploring to the front,
this algorithm was selected due to the movements and time optimization. It was
implemented alongside four data structures: stack of unvisited coordinates, array of visited
coordinates, array of tiles, and array of tiles coordinates.

## Dijkstra Path Planning

For path planning algorithm, this algorithm was used for searching and identifying the
shortest path from the starting tile to the end tile. The path was obtained based on weights
for each tile, and each coord of the path was saved in a stack for easy navigation.

## Tiles

Each Tile object saves important information about each tile, such as victims, checkpoints,
obstacles, black tiles, adjacent tiles and walls direction. Also set weights for diLerent tiles
type, like blue, black, wall and white tiles.

## Victims

Since it can be just one victim per tile, each tile can save up to one victim per tile.
Therefore, a system was implemented for dealing with victims in 90 degrees angle and in
endpoints.

## Map & Button

A map saving function was implemented for handling checkpoints and could retrieve map
information that was already explored. However, this function was not available due to
other problems regarding checkpoint detection. Moreover, a full reset of the map was
implemented in the robot.

On the other hand, the button function was implemented with a time-based system, where
if the button was pressed over half a second, it will restart the ESP microcontroller
reinitializing all the sensors, but when pressed less than this time, it will restart just the
algorithm.

## Ramps

A multiple level maze exploration algorithm was implemented, handling the tiles
assignment when detected a ramp up or down. It was saved inside the map, and the robot
can continue the exploration in multiple levels.

## Main problems

We encounter with diLerent logic and resources problems. Initially we had Stack Overflow
problems due to the RAM of the microcontroller, where we were forced to reduce the
amount of information we can keep inside the ESP to a limited size. Also, a logic problem
emerged when exploring the map, since Tiles where duplicated inside the map and the
solution was to check if the tile exists after added to the map.

## Console Tests – Support diagrams

![Console1](/docs/assets/maze/Console1.png)
![Console2](/docs/assets/maze/Console2.png)

### Algorithm Flowchart

![Algorithm flowchart](/docs/assets/maze/FlowChart.png)

### Maze Graphical Representation

![Maze diagrams](/docs/assets/maze/mazeDiagram.png)

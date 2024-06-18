Similar to last year's implementation, the algorithm of choice for traversing the maze was a **Depth First Search** (DFS) routine using a **Dijkstra's shortest path** implementation for calculating routes between tiles.

##Implementation
During planning, it was decided to first work on a self contained C++ iteration of the code in order to test it only using the terminal. This was, because the programmer in charged of this area wasn't going to have access to the robot prototype during winter break.
Once this was done, a migration to arduino was worked on next. Problems arouse because testing was first conducted using an arduino mega, while the final robot was supposed to use an ESP32 microcontroller and incompatibility between libraries were found. Most of these, were used to imitate data structures from the Standard Template Library which were not available in arduino, which was also a big setback.

##Tiles
A map of the tiles is saved in the robot's memory and the information can be accessed using the x, y and z positions. In each tile data is saved like the availible adjecent tiles, the weight to visit the tile, if a victim has been detected already, if it's floor is black or if its a checkpoint.

##Depth First Search
This type of exploration algorithm prioritices visiting new tiles until a dead end is found before backtracking to visit another tile. This was prefered over a Breadth First Search algorithm that, instead of searching a complete area one by one, seeks to visit other parts of the maze first.

##Dijkstra's shortest path
This famous algorithm is used to determine the best availible path between two coordinates, avoiding blue tiles, ramps and bumpers which would make the shortest path slower. This is a huge time saver compared of simple recursive backtracking.

##Engine
Saving the information of the maze is crucial, because it avoids wasting time going to already visited tiles, dropping uneccesary kits, etc. Keeping track of the robot's position at all times allows us to know the current and adjacents tile's information and finding it's way back to the start in order to achieve bonus points.

##C++ terminal testing
Using a bidimentional characters array we were able to simulate the robot's behavior virtually and test the algorithm's functionality in various casescenarios easly.

| Symbol | Represents |
| ----- | ----- |
| # | Wall |
| > | Move right |
| < | Move left |
| ^ | Move up |
| / | Move down |
| S | Start of path |
| E | End of path |
| r | Ramp inclined right |
| l | Ramp inclined left |
| u | Ramp inclined up |
| d | Ramp inclined down |

![C++ terminal](/assets/maze/c++maze.png)
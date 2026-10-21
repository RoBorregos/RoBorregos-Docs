# Algorithm

It is based on a Depth First Search (DFS) exploration combined with Dijkstra's path planning algorithm. This implementation avoids blue and black tiles while maintaining a record of already explored areas and detected victims through a dynamic tile mapping system.

## Implementation Overview

The algorithm utilizes an iterative DFS approach with a Stack-based structure to avoid memory overhead from recursion. Given the ESP32 microcontroller's limited RAM, data structures are constrained to a fixed maximum size (`kMaxSize`). The system maintains three primary data structures: a stack of unvisited coordinates, an array of visited coordinates, and a tile map with adjacency information.

## DFS Exploration Strategy

The DFS algorithm explores tiles in a priority-based clockwise pattern: Right (90°), Down (180°), Left (270°), Up (0°). This exploration order optimizes robot movement efficiency. The algorithm continuously:
1. Pops the next unvisited tile from the stack
2. Uses Dijkstra's algorithm to find the optimal path
3. Records tile information (walls, obstacles, black/blue tiles)
4. Queues adjacent unvisited tiles for exploration

## Dijkstra Path Planning

Dijkstra's algorithm calculates the shortest weighted path from the robot's current position to a target tile. Each tile edge has an associated weight based on tile type (walls, obstacles, etc.). The algorithm maintains:
- Distance array: tracks minimum distance to each tile
- Previous positions array: enables path reconstruction
- Explored set: prevents reprocessing tiles

The calculated path is stored in a Stack for sequential navigation.

## Follow Path Implementation

The `followPath()` function handles robot orientation and movement along the calculated path. It manages:
- Rotation logic based on target direction (0°, 90°, 180°, 270°)
- Emergency stops via button press detection
- Tile advancement and coordinate updates
- Black tile detection and skipping

## Tiles & Adjacency

Each Tile object maintains:
- Position coordinates (x, y, z for multi-level support)
- Adjacent tile pointers and connection information
- Wall presence by direction
- Weight values for pathfinding
- Special markers: checkpoints, black tiles, blue tiles, victims

## Multi-Level Maze Handling

The algorithm supports ramps between maze levels by:
1. Detecting ramp movement via `robot.rampState` (up: +1, down: -1)
2. Incrementing/decrementing the z-coordinate
3. Creating new tiles at the elevated level
4. Establishing connections between tiles across levels
5. Maintaining the same exploration pattern on each level

## Checkpoint & Button System

A time-based checkpoint system allows:
- **Short press** (<500ms): Restarts only the maze algorithm, returning robot to start position
- **Long press** (>500ms): Full ESP32 restart with sensor reinitialization

During checkpoint reset:
- All data structures reset (visitedMap, tilesMap, tiles)
- Robot returns to initial position and orientation
- Algorithm reinitializes from the starting tile

## Virtual Victims Handling

Victim detection is maintained but commented out in current implementation. The system is designed to:
- Store one victim per tile
- Call detection at specific rotation points
- Handle 90-degree angle victims and endpoints
- Support three victim states: harmed, stable, unharmed

## Main Challenges & Solutions

1. **Memory Constraints**: Limited ESP32 RAM forced fixed-size array implementation instead of dynamic allocation, capping maximum tiles at `kMaxSize`
2. **Tile Duplication**: Early exploration created duplicate tile entries; solution implemented coordinate checking before adding new tiles
3. **Rotation Optimization**: Multiple rotation checks and movement reversals during dead-end handling to minimize unnecessary movements
4. **Blue Tile Handling**: Special connection logic prevents backtracking through blue tiles (one-way passages)

## Debug System

A compile-time debug system (`kMazeDebugEnabled`) allows selective logging without runtime overhead:
- Generic message logging via `MazeDebug()`
- Coordinate logging with labels via `MazeDebugCoord()`
- All debug output through Serial interface
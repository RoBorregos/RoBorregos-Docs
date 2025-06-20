
# Robocup Soccer Open 2025

- [📁 Robot Communication](Communication/index.md)
- [🎮 Robot Control](Control/index.md)
- [🤖 Algorithm Design](Logic/index.md)
- [📸 Robot Vision](Vision/index.md)

### Electronics

Edwin Manuel Camarillo Montero 

### Mechanics

Angela Leticia Rosales Díaz

### Programming


Emil Winkler Partida [@emilwinkp](https://github.com/emilwinkp)

Juan Pablo Briñez Corzo [@brinez-juan](https://github.com/brinez-juan)

## Features

### [Github](https://github.com/RoBorregos/robocup-soccer-open-2025)

🤖 Processing Breakdown:
- Teensy 4.1: Organices data recolected and processed by camara, phototransistors, BNO, and manages them for core logic and movement. 
- OpenMV: Camaras with algorithms for ball and both goals detection using computer vision.

🎮 Movement Cotrol: Used 2 PID for a smother control ensuring a precise movement:

- Translational PID: Guides robot to the ball or goal with camara.
- Angular PID: Corrects the direction of the robot with angular velocity correction.
- Dribbler PID: Adjust robot movement for a soft approach to the ball with the robots front. 
- Ominidirectional movement: Omnidirectional movement using 4 wheels with 90 degree separation. 

⚽Ball detection: Image processing for pixel mapping using LAB values and merged blobs tracking to determine ball distance and angles in a [180, -180] angle normalization. 

🥅Goal detection: Using merged blobs to encapsulate goal in a rectangle with a corresponding center used as reference for distance and angle measurements. 

## Documentation for TMR

[Poster for TMR competition](https://www.canva.com/design/DAGmp68Unzo/DkLrrPsporc31XkUFyraQg/edit?utm_content=DAGmp68Unzo&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)

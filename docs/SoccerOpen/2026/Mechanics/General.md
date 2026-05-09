# Mechanics

## Abstract
The robot is built around a **multi-layer circular chassis**, where each layer serves a specific function. The **bottom layer** houses the drive system and line sensors, the **middle layer** holds the electronics, and the **upper layer** supports the dual mirror vision system. The design prioritizes **modularity** — every component is independently mountable and replaceable without disassembling the entire robot, a key requirement for fast repairs during competition. All 3D printed parts use standardized **M2 and M3 fasteners** throughout the assembly.

## Chassis

### Base Plate
The main circular base plate is the **structural foundation** of the robot. It houses the motor mounts, line sensor PCB positions, and provides the attachment points for the vertical columns. The design includes cutouts at four positions for the motor brackets, holes for the line sensor PCB mounts, and **countersunk screw holes** around the perimeter for the vertical columns, allowing screw heads to sit flush with the plate surface.

| Property | Value |
|----------|-------|
| Material | **Laser-cut acrylic** |
| Thickness | 3 mm |
| Fasteners | M3 countersunk screws |

### Middle Electronics Plate
The middle plate sits above the bottom layer and provides a **mounting surface for the main electronics boards**. It is connected to the base plate via the vertical aluminum columns.

| Property | Value |
|----------|-------|
| Material | **Laser-cut acrylic** |
| Thickness | 3 mm |

### Mirror Support Roof Plate
The top circular plate is mounted on the vertical columns and serves as the **structural base for the entire vision system**. Both mirror plates attach to it from above and below.

| Property | Value |
|----------|-------|
| Material | **Laser-cut acrylic** |
| Thickness | 3 mm |

## Structural Components

### Vertical Columns
Four vertical **aluminum L-profile columns** connect all layers of the robot together and provide the **primary structural rigidity**, replacing the wooden dowels used in the prototype and significantly increasing stiffness under impact.

| Property | Value |
|----------|-------|
| Material | **Aluminum L-profile** |
| Cross-section | 20 × 20 mm |
| Connection | M3 screws bolted through each plate at top and bottom |

### Side Walls
A **cylindrical wall** wraps around the perimeter of the robot between the motor brackets, protecting the internal electronics from **lateral impacts and debris** during matches.

| Property | Value |
|----------|-------|
| Material | ABS or PLA |
| Thickness | 1.5 mm |
| Attachment | Secured between top and bottom plates using M3 screws |

### Handle
A 3D printed handle is mounted on top of the robot for safe carrying and transportation between matches. Competition rules require a **minimum clearance of 5 cm** for hands between the highest non-handle part and the handle itself.

| Property | Value |
|----------|-------|
| Material | PLA |

## Drive System

### Omni Wheels
Four omni wheels provide **full omnidirectional movement**, arranged at alternating separation angles of **90° and 75°**, optimized so that all four wheels contribute to movement in any direction and maximize force in physical duels with opponent robots.

| Property | Value |
|----------|-------|
| Wheel Count | 4 |
| Brand | **GTF Robots** |
| Separation Angles | **90° and 75°** |

### Motor Brackets
Custom **PETG printed brackets** hold each motor in place and connect the upper and lower base plates at the motor positions. The brackets are designed to keep the motors flush and low to maintain a **low center of gravity**.

## Vision System (Mechanics)

The vision system uses a **dual mirror configuration** that provides **complete 360° coverage with no blind spots**, something rarely implemented in RoboCupJunior Soccer Open at national and international level.

### Hyperbolic Mirror
The hyperbolic mirror provides **wide-angle 360° vision of the far field**. Positioned above the camera, it reflects the entire surrounding environment into the camera lens, allowing the robot to see the full field, opponent robots, and goals simultaneously.

| Property | Value |
|----------|-------|
| Mirror Type | **Hyperbolic** |
| Field Coverage | 360° far field |
| Mount | Upper mirror plate, attached to mirror roof plate |

### Ring Mirror
The ring mirror **eliminates the blind spots** that exist in the close range directly around the robot when using a hyperbolic mirror alone. Combined with the hyperbolic mirror, the system achieves **complete visual coverage** from directly adjacent to the robot all the way to the field boundaries.

| Property | Value |
|----------|-------|
| Mirror Type | **Ring / Annular** |
| Field Coverage | Close range 360° around robot |
| Mount | Lower mirror plate, attached below mirror roof plate |

### Camera 1 — Omnidirectional
The primary camera is positioned between the two mirror plates **pointing upward**. It captures the reflections from both the hyperbolic and ring mirrors simultaneously, feeding a single image that contains the **complete 360° view** of the field.

| Property | Value |
|----------|-------|
| Orientation | Upward facing |
| Coverage | **Full 360° via dual mirror system** |

### Camera 2 — Ball Detection
A second **forward-facing camera** is dedicated to direct ball detection, operating independently from the mirror system and providing a direct optical view of the ball's position in front of the robot for **more accurate close-range tracking**.

| Property | Value |
|----------|-------|
| Orientation | Forward facing |
| Function | Direct optical ball detection |

## Line Detection System

Four custom PCBs are distributed **symmetrically around the chassis perimeter** for complete line detection in all directions, ensuring the robot can detect field boundaries regardless of its movement direction or orientation. Each PCB uses a **circular and X-shaped arrangement** of LEDs and phototransistors, a configuration used by high-level international teams.

| Property | Value |
|----------|-------|
| PCB Count | 4 identical boards |
| Sensor Type | **Phototransistors + LEDs** |
| Arrangement | **Circular and X-shaped pattern** |
| Coverage | Full 360° line detection |

## Kicker System

The **electromagnetic kicker** is mounted at the front of the robot. It releases a **high current pulse** through a solenoid to shoot the ball toward the opponent's goal with force and precision.

| Property | Value |
|----------|-------|
| Type | **Electromagnetic solenoid** |
| Position | Front of chassis |

## Recommendations

Even if plates are independent, they are often tethered together by a **web of jumper wires**. Removing the middle layer risks pulling on wires attached to the bottom layer, causing **broken pins or loose connections**. Additionally, accessing the bottom layer frequently requires removing the top and middle layers first, as the vertical columns and cylindrical side walls **block hand access**.

For future iterations, it is recommended to design dedicated **cable management solutions** that decouple each layer electrically before physical separation, and to evaluate **access panel designs** on the side walls that allow bottom layer maintenance without requiring full disassembly.

## Resources
You can access all mechanical files used in the development by following this [drive link](https://drive.google.com/drive/folders/1iLI8nZqHYHWCFtoLrhFHCsU3mt8n4tq5?usp=sharing).
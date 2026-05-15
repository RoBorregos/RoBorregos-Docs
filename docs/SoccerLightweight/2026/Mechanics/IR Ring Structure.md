## IR Ring Structure

The IR ring was designed as one of the most important subsystems of the robot, since it directly affects the quality and stability of ball detection. Its primary purpose was to support and position the TSOP infrared sensors while also improving the quality of the received signal.

The structure surrounding the sensors helps partially isolate them from unwanted external interference and excessive side-angle light, improving directional sensitivity and reducing unstable readings caused by reflections or environmental lighting conditions.

Additionally, the ring was designed to function as a structural gripping point for handling and transporting the robot during maintenance, testing, and competition preparation.

During experimental testing, it was observed that the vertical position of the IR ring had a major influence on angular accuracy. Several tests demonstrated that lowering the ring closer to the ground significantly reduced the angular error during ball tracking.

The main reasons observed for this improvement were:

- Better alignment with the infrared-emitting ball
- Reduced signal dispersion
- Lower reflection interference
- Improved sensor consistency during movement

Because of this, the final design positioned the IR ring as low as mechanically possible without compromising mobility or structural stability.

> **Note:** Experimental testing showed that lowering the IR ring improved tracking accuracy more effectively than applying additional software filtering methods.

One of the principal development challenges was the incorrect mapping of several IR sensors around the ring. This issue originated primarily from communication inconsistencies between the mechanical and electronics development areas.

Differences appeared between:

- Physical sensor placement
- PCB routing positions
- Software indexing of the sensors

As a result, some sensors were incorrectly associated with angular positions inside the software, generating unstable directional estimations during early testing phases.

This problem caused:

- Incorrect angle interpolation
- Sudden angular jumps
- Unstable triangulation behavior
- Inconsistent ball tracking

The issue was later corrected through iterative calibration and remapping procedures, highlighting the importance of synchronization between mechanical design, electronics integration, and software implementation.

> **Warning:** Small mismatches between physical sensor placement and software indexing can produce large angular estimation errors even when the sensors themselves operate correctly.

In the final physical implementation, an Arduino board was mounted on the upper section of the robot near the IR ring assembly.

Originally, the design intended to integrate all processing electronics directly into the custom PCB system. However, due to limitations encountered during the embedded PCB integration process, the Arduino had to remain externally mounted.

The primary reasons for this decision included:

- PCB routing limitations
- Embedded integration constraints
- Simplified debugging access
- Rapid prototyping requirements
- Development time limitations

Although this solution increased the exposed wiring and occupied additional space in the upper section of the robot, it considerably simplified debugging and accelerated testing iterations during development and competition preparation.

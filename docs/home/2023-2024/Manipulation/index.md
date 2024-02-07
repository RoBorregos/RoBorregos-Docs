# Manipulation
### Planning and Hardware
- Replaced Xarm5 for a Xarm6, as well as designed new custom grippers.
- Migrated the planning and arm control computation to the Jetson AGX Xavier, for stable compute times and a more reliable ethernet-based communication with the arm.
- Started a migration from MoveIt planning to faster alternatives.

### Pick and Place
- Increased accuracy and stability of the pick process.
- Developed two place methods, one with a graph based approach and another using clustering for identifying possible placing areas.
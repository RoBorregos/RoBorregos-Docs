# Achievements from 2024

This year, after being [accepted](https://athome.robocup.org/2024-qualified-teams/) to participate in RoboCup 2024 held in Eindhoven, Netherlands, the team focused in developing a robust and reliable set of software modules, tailored to the specific needs for the tasks for the updated rulebook. This approach, had the purpose of showcasing a functional robot for both the Mexican Robotics Tournament (April; Monterrey, Mexico) and the RoboCup (July; Eindhoven, Netherlands).

The robot was renamed as FRIDA (Friendly Robotic Interactive Domestic Assistant), an acronym reflecting the purporse of the robot, and the name in reference to Mexican culture.

With the vast knowledge acquired during the international tournament, the team defined the new objectives for the remainder of the year to be: an increased focus in research and literature review, and centralized and offline refactorization of the software and systems.

These are the major achievements divided by area:

## [Integration](Integration/index.md)
### Software Architecture
- A new empty repository was created, alongside separate repositories for each area, added as submodules of the first. This allowed us to work of different branches of development in the central computational unit (Jetson AGX inside robot).
- Every submodule was containerized, using Docker, to ease the installation process and dependency management.

### Task manager
- A new package named `task_manager` was created to handle the flow of each task using general python submodules per area.
- There's a Python ROS node for each task, following a state machine architecture.

## [Human-Robot Interaction](Human-Robot%20Interaction/index.md)
### Natural Language Processing
- Following the update of GPSR task, a new model was fine-tuned to cover for more complex scenarios, using GPT3.5 API as the base
- The package `frida_language_processing` was created, with the command interpreter capable of storing context throughout the execution, a cleaner implementation of the embeddings match process for actual items in the scene, and specific features for the **Receptionist** task.

### Speech
- Keyword spotting (@Oscar-gg)

### Display
- Nadota (@FranciscoSP)

## [Computer vision](Computer%20vision/index.md)
- Nadota (@Ale-Coeto)

## [Manipulation](Manipulation/index.md)
## Placing Algorithm with Gaussian Distribution

To optimally place objects, we:

1. **Isolate the Table Surface**:
   - Use **RANSAC** to extract the table plane from 3D point cloud data.

2. **Generate a Heat Map**:
   - Apply a **Gaussian distribution** over the surface to create a heat map highlighting open spaces.

3. **Select Optimal Placement**:
   - Choose the area with the highest value on the heat map as the placing pose.

4. **Plan Collision-Free Trajectory**:
   - Use **MoveIt!** to plan a safe path to the placing pose.

This concise approach ensures efficient space utilization and safe object placement.


## [Navigation](Navigation/index.md)
- Nadota (@deivideich, @Chapa-1810)
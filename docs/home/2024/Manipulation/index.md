# Manipulation

Manipulation is one of the most essential and challenging aspects of robotics, enabling robots to physically interact with their environment to perform meaningful tasks.

## Our Innovative Pipeline

We have developed a pipeline that empowers a 6-degree-of-freedom robotic arm and gripper to execute object manipulation by seamlessly integrating:

- **Image Processing**
- **Perception**
- **Motion Planning Components**

## Object-Picking Phase

- **Data Fusion**: Enhance 2D object detection data with 3D point cloud information using the **Point Cloud Library (PCL)**.
- **Plane Extraction**: Utilize **RANSAC algorithms** to extract planes from the point cloud.
- **Object Segmentation**: Segment clusters corresponding to detected objects.
- **Grasp Planning**: Process detections with the **Grasp Pose Detection (GPD) library**.
- **Trajectory Generation**: Use **MoveIt!** to generate collision-free trajectories for grasping.

## Motion Planning

- **Primitive Movements**: Employ primitive movements for pick-and-place operations.
  - **Benefits**:
    - Faster planning.
    - Maintains the end effector's orientation.
    - Ideal for tasks like serving breakfast that don't require collision avoidance.
- **Collision Avoidance**: When necessary, alternative planning methods are used to navigate around obstacles.

## Placing Algorithm

- **Surface Isolation**: Isolate the table surface using **RANSAC**.
- **Heat Map Creation**: Generate a heat map of available space based on a Gaussian distribution.
- **Optimal Placement**: Select the region with the maximum accessible space as the placing pose.

<!-- Manipulation is one of the most essential and challenging aspects of robotics, enabling robots to physically interact with their environment to perform meaningful tasks. Our innovative pipeline empowers a 6-degree-of-freedom robotic arm and gripper to execute object manipulation by seamlessly integrating image processing, perception, and motion planning components. In the object-picking phase, we enhance 2D object detection data with 3D point cloud information using the Point Cloud Library (PCL) and RANSAC algorithms to extract planes and segment clusters corresponding to detected objects. The Grasp Pose Detection (GPD) library processes these detections, while MoveIt! generates collision-free trajectories for grasping. For motion planning, we utilize primitive movements for pick-and-place operations, ensuring faster planning and maintaining the end effector's orientation—perfect for tasks like serving breakfast that don't require collision avoidance. When collision avoidance is necessary, we employ alternative planning methods. The placing algorithm isolates the table surface using RANSAC, creates a heat map of available space based on a Gaussian distribution, and selects the region with the maximum accessible space as the placing pose. -->

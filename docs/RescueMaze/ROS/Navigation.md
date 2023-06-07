# Navigation

A navigation system was developed for a more flexible robot in terms of adaptability, since using a real map can help get more precision compared to a system that uses unitary movements.

The implementation was done using the navigation stack provided by ROS. To get the setup running there are some requirements that need to be met, such as the sensor sources, the transform configuration, the odometry information, the base controller, the map, the robot model, and the navigation stack itself.

## Sensor sources

The sensor sources are the sensors that the robot uses to get information about its surroundings. In this case, the robot uses a 2D LiDAR. In this case, a LD06 LiDAR was used initially, but due to the noise in the readings, it was changed to a YDLIDAR Tmini Pro. To use the LiDAR, each one had its own ROS package, which was used to get the information from the sensor and publish it to a topic. The packages used were:

- [ld06](https://github.com/ldrobotSensorTeam/ldlidar_stl_ros/tree/964afad8b41ca7ee1a66e5b37acaafdc55190ea8)
- [ydlidar](https://github.com/YDLIDAR/ydlidar_ros_driver/tree/1439d7a9f1a276ed3383af1830130c2074d46001)

Each package has its own launch file, which is used to launch the node that publishes the information to the topic `/scan`. The launch files used were:

- [ld06.launch](https://github.com/ldrobotSensorTeam/ldlidar_stl_ros/blob/964afad8b41ca7ee1a66e5b37acaafdc55190ea8/launch/ld06.launch)
- [Tmini.launch](https://github.com/YDLIDAR/ydlidar_ros_driver/blob/1439d7a9f1a276ed3383af1830130c2074d46001/launch/Tmini.launch)

Another sensor used was the IMU, which was used to get the orientation of the robot. The IMU used was the BNO055, which wass used with the package [bno055](https://github.com/dheera/ros-imu-bno055/tree/4c80f360a7dce3abefeb8df505d2331f29c3fcc0). The launch file used to launch the node that publishes the information to the topic `/imu/data` was: 

- [imu.launch](https://github.com/dheera/ros-imu-bno055/blob/4c80f360a7dce3abefeb8df505d2331f29c3fcc0/launch/imu.launch)

<!-- ## Transform configuration

The transform configuration is used to get the information from the sensor and transform it to the base frame of the robot. In this case, the transform configuration was done by creating an URDF file for the robot. The URDF file was created using the [URDF tutorials](http://wiki.ros.org/urdf/Tutorials) provided by ROS. The URDF file was created using the [xacro](http://wiki.ros.org/xacro) package, which is used to create macros for the URDF file. The URDF file was created using the following packages:

- [xacro](http://wiki.ros.org/xacro)
- [robot_state_publisher](http://wiki.ros.org/robot_state_publisher)

These packages were used to create the URDF file and publish the information to the `/tf` topic. The launch file used to launch the robot state publisher was: -->


## Map

The map was created using the Hector SLAM package, which is a SLAM algorithm that can be used without odometry. The map was created using a 2D LiDAR. To launch the mapping process, the following command was used:

```bash
roslaunch nav_main hs_mapping.launch
```

This command launches the mapping process and the RViz visualization. The mapping process is done by moving the robot around the environment, so that the LiDAR can get information about the environment. 

---

## Planning

The planning is done using the navigation stack provided by ROS. Once setup, the navigation stack receives information from different sources, as well as different goals from the [algorithm](../Algorithm.md). Upon receiving a goal, the navigation stack plans a path to the goal and calculates the velocity commands to send to the robot. In this case, the planner used was the dwa_local_planner, which is a local planner that uses the Dynamic Window Approach to calculate the velocity commands. The velocity commands are calculated using the information from the sensor sources, the odometry information, the map, and the robot model. 

The velocity commands were initially published to the `/cmd_vel` topic, which is used by the base controller to move the robot. However, in order to utilize the limit switches, the velocity commands were published to another topic, and another node was created to listen to the velocity commands from the planner, as well as the velocity commands published by the algorithm (recovery situations), and published to the `/cmd_vel` topic prioritizing the recovery situations. The node used to do this was the mux_cmd_vel custom node.


## Sending goals

The goals are sent to the navigation stack using the [move_base](http://wiki.ros.org/move_base) package. In this case the goals were limited to the goals sent by the [algorithm](../Algorithm.md), being only 90 degree turns and 30 cm movements forward or backward. In order to send accurate goals, a custom transform was used, which is used to represent the ideal position of the robot at any given moment, compensating for incaccuracies in the robot's translational and rotational movement. 

This transform was calculated by using the IMU yaw data as well as the [localization_grid](/docs/RescueMaze/ROS/LocalizationGrid.md) data.

- IMU data: Stored when the robot is initialized, and used to calculate the angle of each cardinal direction, which are then used to update the transform.
- Localization grid: Used to get the distance from the robot to the center of the current tile. Used to update the transform to send goals from the center of the tile.


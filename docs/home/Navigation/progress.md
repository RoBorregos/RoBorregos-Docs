# Navigation Progress June 2023 | RoBoregos @Home
The most updated code can be found at the branch ```feature/nav``` of the repository.
- [Link to branch](https://github.com/RoBorregos/robocup-home/tree/feature/nav)

<br/>

For the competition we dispose of two robots, one which is the DashGo B1 as a base with the xArm mounted, and another one which is the SCARA. It is important to know that different configurations are needed for each robot, and it is important to remember that the DashGo is a differential robot, that is, it is a robot with differential traction that uses a transmission system of two independent wheels, so each wheel is attached to its own motor, and the DashGo B1 has in this case two idle wheels. On the other hand, the SCARA robot has four independent wheels, but its wheels are omnidirectional, so the robot has a holonomic system.

In terms of progress, in the first semester of 2022 we worked for the first time on the SCARA navigation, we were able to map and navigate, but the navigation was not the most optimal. We stopped working on the SCARA to improve its mechanics, so in the first semester of 2023 we worked with the DashGo B1 base, where we have a very good navigation, but as mentioned, the navigation can always be improved.

### Cloning the repo
Make sure you have ROS Noetic which runs in Ubuntu 20.04
<br/>

```git clone --recurse-submodules https://github.com/RoBorregos/robocup-home.git```
<br/>

Now lets change from branch from ```develop``` to ```feature/nav```
<br/>

```git switch feature/nav```
<br/>

```git pull```
<br/>

```git submodule update --init --recursive```
<br/>

You will have to install packages and dependencies. 

<br/>

The principle scripts and launch files for navigation can be found at [nav_main](https://github.com/RoBorregos/robocup-home/tree/feature/nav/catkin_home/src/navigation/nav_main)<br/>

The launch file for mapping with the DashGo with an IMU sensor is [gmapping_imu.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/navigation/nav_main/launch/gmapping_imu.launch)<br/>

The launch file for navigating autonomously towards a goal with the DashGo with an IMU sensor is [navigation_imu_2.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/navigation/nav_main/launch/navigation_imu_2.launch)<br/>

No tests have been made with the new SCARA robot, so this is a next challenge, you can use as base guide the [navigation_imu_arduino.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/navigation/nav_main/launch/navigation_imu_arduino.launch) for the NavStack part, and for mapping either [gmapping.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/navigation/nav_main/launch/gmapping.launch) or [gmapping_tmr.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/navigation/nav_main/launch/gmapping_tmr.launch) <br/>

A difference to notice between the files for the DashGo or the SCARA is the ```driver_imu``` file, which contains the initial run up for each robot, where you could run ```teleop_twist_keyboard``` to check it is moving correctly.<br/>
You cand find the driver file for the DashGo at [catkin_home/src/dashgob1_noetic/src/dashgo/dashgo_driver/launch/driver_imu.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/dashgob1_noetic/src/dashgo/dashgo_driver/launch/driver_imu.launch) and the one for the SCARA or the DashGo's we modified can be found at [catkin_home/src/navigation/base_control/launch/driver_imu.launch](https://github.com/RoBorregos/robocup-home/blob/feature/nav/catkin_home/src/navigation/base_control/launch/driver_imu.launch)<br/>
# Planning and Hardware

In our latest iteration, we replaced the Xarm5 arm with a Xarm6. This upgrade provides an extra degree of freedom (DOF). Importantly, both arms have public `xarm-ros` packages for planning with MoveIt.

Another significant change was mounting the Zed2 stereo camera directly on the arm's last axis. Previously, its static position at the robot's base frequently obstructed trajectories during the planning process, causing delays. To decrease latency in arm motion, we now run MoveIt on the Jetson Xavier AGX.

![Xarm6 with Zed2](/assets/home/Manipulation/xarm.png)
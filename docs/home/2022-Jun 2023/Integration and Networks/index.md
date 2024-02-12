# Overview

The area of integration and networks is concerned about how all the modules are connected, including hardware and software. 

The area is in charge of do the state machines of the tasks for Robocup@Home, remember to read frecuently the [Robocup@Home Rulebook](https://robocupathome.github.io/RuleBook/rulebook/master.pdf) to be aware of the rules of the competition.

In the following sections, we will discuss the following topics:

- [Jetson Xavier AGX](Jetson Xavier Agx.md)
- [Jetson Nano](Jetson Nano.md)
- [Network](Network.md)

Integration is a key part of the project, since it is the way to connect all the modules and make them work together. You must have an idea of how the modules are connected and how they communicate with each other. And also how all the modules are working.

Also you need to be aware of hardware issues and when you need to change some hardware configuration, for example, if you need more jetson nano devices, in order to have more local processing power.

## Considerations of all the modules in the network

### Navigation

The navigation module is based on a ROS package called [ROS Navigation Stack](http://wiki.ros.org/navigation), it is a 2D navigation stack that takes in information from odometry, sensor streams, and a goal pose and outputs safe velocity commands that are sent to a mobile base. Regularly you have a move_base node that is the one that is in charge of the navigation, it takes the goal pose and the odometry information and it sends the velocity commands to the mobile base. 

### Human Robot Interaction

The natural interaction with the user is something that you need to consider in the integration, you need to know how the user is going to interact with the robot, and how the robot is going to respond to the user. Also what the robot will do if he doesn't understand the user.

#### Speech

You need to fully understand the calls of the speech module, you can check specific detalis in the [Speech Module](../Human%20Robot%20Interaction/speech/index.md). Remember all the control topics and be aware of any misspelling thats why is recommended to have a confirmation call for every command given by the user.

#### Human Analysis

The detection or the analysis of the human should be storaged and tagged correctly, the natural implementation of the HRI area should be consider when integrating the system

### Manipulation

Remember to always check the hardware configurations for any manipulation task and remember that the area of manipulation is in charge of the manipulation of the objects, remember the safety of the user and include routines for different cases from a the object falling or the user not getting the object.

### Vision

With vision you can check different objects of the environment, and integrate them to any behavior, just rememberthe latency of the vision module and consider the control topics of this module.
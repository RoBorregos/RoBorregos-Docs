# Display

In order to allow interaction with the robot and receive visual feedback, a diplay was integrated to the robot.

<p align="center">
<img src= "/assets/home/HRI/Display.png" alt= "Display" width="80%" height="80%">
</p>

## Interface
For full customization, the interface was developed as a Next.js web application, using Typescript, React and TailwindCSS. In addition, to connect to ROS, `roslib` was used. This is a Javascript library that uses websockets to connect with rosbridge, allowing for the web application to both publish and subscribe to ROS topics.

## Features
Users can interact with the display through a navigation bar that allows access to different sections:

- **Commands:** Taking advantage of the main engine, this section shows basic commands for the robot to follow, these being: Go, Grab, Put, Find, Introduce and Stop, each having further options or locations accordingly in order to publish a topic with the desired command.

- **Camera:** Shows the Zed camera feed, with the option to visualiza human recognition or object detection frames.

- **Navigation:** Still in development, this section allows the user to visualize the the navigation status through RVIZ web.

- **Status:** Shows basic information about the robot, such as current task or debugging messages.

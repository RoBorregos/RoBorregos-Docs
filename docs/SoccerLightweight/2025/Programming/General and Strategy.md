# General

We organized our codebase into modular libraries utilizing Github for collaborative work. Each library corresponds to a specific hardware component, this was done in order to improve code readability and make a more maintainable and scalable system. The considerations for modularity have let us debug our code quickly and efficiently. 

For robot programming, we use Visual Studio Code together with the PlatformIO extension, which allows us to work in a more organized and professional way with microcontrollers like Teensy and Arduino. PlatformIO provides support for multiple boards, code autocompletion, version control, and better library management, which significantly enhances our development workflow.
We also use GitHub as our platform for file management and team coordination. It hosts all our code and design files online, making them accessible to all team members at any time and location. This ensures that everyone always works with the latest version and that collaboration runs smoothly.
Additionally, we use GitHub Desktop, a graphical interface that simplifies the use of GitHub, especially for members who are less familiar with command-line tools. We publish both our software and hardware developments there

Link:[GitHub Repository](https://github.com/RoBorregos/Soccer-Lightweight-2025.git)

## Tools

The main tools used to program the robot are:

-Visual Studio Code
 (IDE with the PlatformIO extension)
-Pixymon (Software used to configure the Pixy Camera)

## Strategy

As part of our strategy, we deployed two specialized robots: one for offense and one for defense.
The attacking robot used an IR ring to track the ball by following its infrared signals. With the assistance of the camera, it calculated the angle to the opponent’s goal and navigated in that direction to attempt a score. Phototransistor plates were used to prevent the robot from entering the goal area.

For the defending robot, the camera was oriented toward our own goal. It was also equipped with an ultrasonic sensor. Working together, the camera and sensor defined a movement zone, allowing the robot to track and follow the ball within a limited defensive area.

## Algorithm

### **Attacking Robot**


### **Defending Robot**
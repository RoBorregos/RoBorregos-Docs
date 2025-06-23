# General

![foto](images/Horchata_jamaica.jpg)

We structured our codebase into modular libraries and used GitHub to support collaborative development. Each library corresponds to a specific hardware component, which improves code readability and contributes to a more maintainable and scalable system. This modular approach has also allowed us to debug efficiently and make quick adjustments when needed.

For programming the robots, we use Visual Studio Code along with the PlatformIO extension. This setup provides a more organized and professional environment for working with microcontrollers such as Teensy and Arduino. We suggest to read our documentation on how did we use platformIO on our GitHub Repository

GitHub serves as our central platform for version control, file sharing, and team coordination. It hosts all our software and hardware files, making them easily accessible to every team member regardless of time or location. This ensures everyone is always working with the most up-to-date versions and helps maintain smooth collaboration across the team. Also it helped for mentors to check our updates on software development for the contest

Link: [GitHub Repository](https://github.com/RoBorregos/Soccer-Lightweight-2025.git)

## Tools

The main tools used to program the robot are:

-Visual Studio Code (IDE with the PlatformIO extension)  
-Pixymon (Software used to configure the Pixy Camera)

## Strategy

As part of our strategy, we deployed two specialized robots: one for offense and one for defense.
The attacking robot used an IR ring to track the ball by following its infrared signals. With the assistance of the camera, it calculated the angle to the opponent’s goal and navigated in that direction to attempt a score. Phototransistor plates were used to prevent the robot from entering the goal area.

For the defending robot, the camera was oriented toward our own goal. It was also equipped with an ultrasonic sensor on the back of the robot. Working together, the camera and sensor defined a movement zone, allowing the robot to track and follow the ball within a limited defensive area.

## Algorithm

### **Attacking Robot**


### **Defending Robot**
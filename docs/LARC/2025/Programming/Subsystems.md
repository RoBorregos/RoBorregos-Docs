# Subsystems

Subsystems are the basic unit of robot organization. They are responsible for a specific task/physical/mechanical mechanism that is part of the robot. A subsystem is an abstraction for a collection of robot hardware that operates together as a unit. Subsystems form an encapsulation for this hardware, “hiding” it from the rest of the robot code and restricting access to it except through the subsystem’s public methods. 

To achieve this, a common interface is defined for all subsystems, from which each subsystem inherits and implements its own specific behavior. This interface is responsible for opening a common communication between the subsystem and the rest of the robot code, allowing for subsystems to be used in a consistent way.

The robot subsystems are:

- Communication
- DistanceSensors
- Drive
- Dropper
- Elevator
- Intake
- LineSensor
- Sorter
- Vision

While each subsystem is responsible for a specific task, the interface provides the following methods:

```cpp
void begin() // All IO/Hardware initialization must be done here 
void update() // This method is called every time per robot loop
void setState(int State) // Common method for chaning states and managing the subsystem state see State Machine section for more information
```

These methods allowed for a consistent and predictable behaviour of the subsystems, making it easier to debug and maintain the code.

Note: The begin() method must be called when the robot is initialized, commonly in the setup() method on standard Arduino code. This is because pinMode() and other IO initialization requires certain timing and libraries to be loaded before calling these methods. 
# Mechanical Documentation - LARC Robot 2026
## Hans Velarde

# Mechanical Overview
The different systems in the robot were developed by iterating on the previous version used in 2025 by the team. As with any design process, we first defined the requirements and game strategy that the robot needed to follow. Starting from last year’s robot allowed us to identify issues and improve them, resulting in a better and more refined version.

<p align="center">
  <img src="chassisCompleto.jpeg" width="650">
</p>

The full mechanical design and all assemblies were modeled using SolidWorks CAD software. The main materials used include aluminum profiles for rigid structures like the chassis frame, and laser-cut MDF plates for parts such as the storage boxes and elevator structure. We also used 3D printing for more complex and flexible parts, such as elevator ramps and bar holders.

The final mechanical design is made up of five modular subassemblies that come together to form the complete robot. One of the main advantages of this modular approach is that it allowed us to work on different subsystems at the same time. For example, we could prototype and improve the elevator separately from the chassis. This also allowed electronics and programming to progress in parallel with mechanical manufacturing and assembly.


---

# Chassis

<p align="center">
  <img src="chassisCompleto.jpeg" width="650">
</p>

One of the biggest changes compared to the previous version was replacing the mecanum wheel chassis with an omnidirectional drive system. The chassis is one of the most critical parts of the robot, and this Omni system is widely used because it allows movement in any direction and gives better control and maneuverability.
The motors are mounted on a rigid MDF base, which provides stability and prevents misalignment, ensuring smooth and precise movement. 20 mm aluminum profiles reinforce the structure and support both the drivetrain and upper assemblies. At the front and back, MDF plates are used to mount distance sensors such as VLX, QTR, and IR sensors. These are attached using 3D-printed mounts that allow easy calibration and replacement.
The upper structure also uses aluminum profiles to support the storage system and provide safe access to electronics, making maintenance easier. Additional external mounts were designed for sensors like IR and QTR so they extend beyond the wheel perimeter, ensuring proper line tracking while keeping the wheels inside the field boundary.


<p align="center">
  <img src="chassisAbajo.jpeg" width="650">
</p>

### Weight Distribution and Stability

<p align="center">
  <img src="soloBajoChassi.jpeg" width="650">
</p>

---

# Elevator

<p align="center">
  <img src="Elevator.jpeg" width="500">
</p>

The elevator system uses a piston driven by a 12V DC motor. It can generate up to 1000 N of force, with a lifting speed of 10 mm/s, a total weight of 1350 g, and a 300 mm stroke length. This system was chosen because it is simple, reliable, and compatible with the rest of the robot since it uses the same operating voltage as the chassis motors.
During the final assembly, we discovered an unexpected issue not mentioned in the datasheet: the piston could rotate around its axis. To fix this, we designed a custom 3D-printed guide installed at the top of the elevator, preventing unwanted rotation and ensuring a single degree of freedom.
The support structure was built using hollow aluminum profiles and acrylic parts, which made assembly easier and improved overall stability. This piston-based design is compact and strong enough to lift the intake and guiding systems while keeping the structure simple and robust.


<p align="center">
  <img src="camaraElevator.jpeg" width="500">
</p>

---

# Collection System

Based on the strategy of collecting two levels in a single pass, the collection system was designed to capture coffee beans from different heights and guide them into a single path, regardless of their level. This is challenging because it must work within a limited space defined by the elevator.
Two SG995 servomotors are used, controlled by camera vision. Depending on what the camera detects, the servos move simple 3D-printed paddles that either block or allow beans to fall. Gravity then directs the beans into ramps that lead to the next subsystem, the classifier.
Instead of using complex grippers, the system relies on simple, fast-moving paddles that either knock down beans when needed or stay out of the way when not required.


---

# Classification System

<p align="center">
  <img src="Sorter.jpeg" width="550">
</p>

This subsystem connects the collection system with storage and is responsible for sorting coffee beans into category A or B.
As beans travel through a 3D-printed tunnel, they are arranged one by one in a controlled path. At the end of the collection ramp, a SG995 servo holds the beans in place using a gate. A camera positioned above the tunnel identifies the bean’s category.
Once the vision system determines the correct path, the servo opens the gate so a single bean passes through. It is important that both the mechanical system and software work together so only one bean is processed at a time, and the process repeats for each new bean.
The second part of the system uses another SG995 servo that rotates a ramp to two fixed positions, directing beans to the correct storage container. For one category, an additional ramp guides the beans into their final storage path.


---

# Storage System

<p align="center">
  <img src="SorterBox.jpeg" width="550">
</p>

The storage system consists of two MDF containers made with laser cutting, allowing them to be assembled like boxes without many screws or fasteners.
Inside, each container has a small ramp and a 3D-printed circular guide that ensures the beans always roll toward the exit in a centered and controlled way. One container has a larger capacity than the other to match the sorting strategy from the classifier.
To release beans into the correct container, a rack-and-pinion mechanism is used. A 3D-printed gear driven by a servo moves along a toothed MDF rack. By moving in one direction, one container is opened, and by moving in the opposite direction, the other container is activated.


---

# Mechanical Improvements and Suggestions

Consideration of buying new motors, as the ones used have shown latency and can malfunction at any moment. New motors will not only improve the control of the robot, but the general navigation of it.

New Omni wheels can be used, as the chassis has four old wheels from Vex. The rods are already fatigued and worn down. The shaft also has mechanical play, so the control is never as precise as it should be. We used blue painter tape to try and control the game, but usually got loose. 

The intake prints should fall from the robot with no trouble, as the current mechanism and in the National was with hope and faith that they will fall to its position with gravity. 

Cable management must be considered from day 1 of doing the CAD. From where the PCBs will be, how the camera will be held onto, and how the cables will be organized. This helps not only the electronics but the general way in which the robot will be organized. It will also avoid previous mistakes of assembling physically the robot and then try ways to fit electronic components and cables, which led to randomly making holes, using tape and zipties. 

The entirety of the sorting system is the mechanism that must be the most improved and worked on. After the Nationals, the mechanism was never even used, highlighting that it is still not proven to work and can therefore be redesigned. For the international, researching and testing the best mechanism for the sorting is required, taking into consideration the simplest way in which to sort the fastest way with the least amount of materials and actuators.

The back QTR holder must be redesigned to be placed more inside towards the chassis, as it will ensure the robot can align better when dropping the beans in the corresponding baskets.  
The 3D prints of the sensor can be reworked and improved, mainly for their best accessibility for electronics. 

Using GrabCad and other open source websites for finding CAD models of sensors and hardware always help out to produce the most precise CAD part modeling and help in time saving. Onshape has better support for generating custom gears and racks if working in these designs. 

To avoid any material loss when printing, always ensure to test clearances and the scale of the print. For example, if you will print the first version of the IR holders, which you need 4 or more, first print one, test, and improve upon it. Chances are minimal that the print will always turn out exactly as you intend, so avoid water of filament and print time. 

When designing in Solidworks, try the best possible to work on subassemblies. Solidworks is a very demanding software computer-wise, so to avoid unexpected crashes, working with fewer parts at a time will help keep everything running smoothly. 

This also helps to design more modular assemblies that can be later in real life individually manufactured and assembled. Always save Solidwork files every once in a while and keep backup files.

For laser cutting of the MDF boards, use the laser machine in the Innovaction. For cutting profiles or custom metal parts, use the Mechatronics lab in A7. 

Communication with programming and electronics is key. At the end of the day, the mechanics must always consult them when designing, taking into consideration their area needs and how you can incorporate them. 

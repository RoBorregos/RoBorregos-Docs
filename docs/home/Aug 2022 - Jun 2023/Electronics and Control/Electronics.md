# Electronics

## Main objective
<div align="justify">
Electronics development in Roborregos consists in the ideation, design, evaluation, construction and implementation of solid electronic systems which must be able to apply proficient control over different actuators and allow robust communication among sensors, modules and microcontrollers.
</div>  

## Metodology

<div align="justify">
In order to build a solid electronic system it is compulsory to develop custom PCBs (Printed Circuit Boards). There are multiple steps that must be followed in order to avoid critical mistakes in the final PCB design, and though there is no a single way to make a PCB, these fundamentals steps must be followed without skipping under any circumstances:
</div>
  
- Research every component involved in the system

<div align="justify">
Even before drawing connections or schematics, it is obligatory to research every component that will be used. Application notes or technical PDFs are the main source of information to understand how the selected devices work, and what must be considered to ensure their best operational conditions. Also, this is the only way to prevent accidents related to power supplies, bad connections, or inappropiate use of components.
</div>

- Make a diagram of connections

<div align="justify">
After researching about the components that will be used is crucial to specify in a graphic way how they are going to be connected with each other. This ensures that everyone involved in the project understands how the relationship between the components and how to avoid hazards from incorrect connections.
</div>

- Test the conections in a protoboard or perforated board

<div align="justify">
No PCB is ordered without previous testing because of unexpected behaviors that componentes may have due to the real-world conditions involved in the circuitry. Though most of the fundamental calculations can be done manually (like the total amperage consumption or voltage drops in certain components) there's simply no way of proving that a design will work if there is no proof of it's functionality in a protoboard or a perforated board. 

<br/>
<br/>
Building the desired circuit and testing it it's the only way to know with precision if it's going to work in a PCB, and also, which issues may have under certain conditions. However, this rule may not apply to the implementation of MCUs (microcontrollers) in SMD packages due to physical limitations or other specific components which might not be able to be tested in conventional ways.  
</div>

- Ask someone else to evaluate your design or to check for improvements

<div align="justify">
It is important to get every PCB evaluated by at least another person which understands its purpose and functionality. Ideally, in @home every electronic must know their PCBs and the principles to determine their quality, so always take in consideration that there's room for improvement and the ideas that might come from discussion will be beneficial for the robot's development.

</div>

## Considerations

<div align="justify">
Each PCB must comply with specific requirements to avoid supply, shortcut and efficiency issues according to it's purpose. 
<div>



The main process followed for electronics application comes like this: Desgin/Circuit building/Connection.

First of all the design of the connections between modules.

In order to get a full control of the robot it is fundamental to understand how it is composed

The scara-based robot has omnidirectional movement which comes from the usage of 4 mecanum wheels, powered by 4 Pololu's HP motors. 





Then for the design process we use EasyEda .


For the building process, we use the next standars for general purposes:



For the connectors standars we have: 


The cable selection was made giving the next parameters:

</div>
</div>
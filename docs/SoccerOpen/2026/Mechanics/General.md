# Mechanics

## Mechanical Structure
The mechanical structure was designed to provide a rigid, lightweight, and modular platform capable of integrating all electronic and electromechanical subsystems reliably. The design process began in December 2025 with a prototype built from MDF plates and PLA printed parts to validate the general geometry and component layout. During testing, the prototype revealed critical limitations: the structure lacked sufficient rigidity, cable management was difficult to maintain, and the integration between mechanical and electronic systems required significant improvement.

For the final version, the base plates were migrated to laser-cut acrylic, vertical columns were fabricated from aluminum L-profile to provide greater structural rigidity, and key structural components were reprinted in ABS for its superior impact resistance. PCB mounts were designed as independent printed pieces that bolt directly onto the base plate, enabling easy repositioning and maintenance of electronic components during competition.

### General Recommendations
For future iterations, it is recommended to design dedicated access panels or removable sections on the structure to allow faster and easier access to electronic components without requiring full disassembly. Improving cable routing channels within the structure would also reduce interference with moving parts and simplify troubleshooting during competition.

## Vision System (Mechanics)

The vision system was designed around a dual mirror architecture to achieve complete visual coverage of the playing field with no dead zones. The primary camera operates in conjunction with a hyperbolic mirror, providing wide 360° far-field omnidirectional vision of the environment. A secondary ring mirror was incorporated to eliminate the blind spot inherent to hyperbolic mirror systems in the area closest to the robot, ensuring full spatial awareness at all ranges. A third forward-facing camera was dedicated exclusively to ball detection, operating in the horizontal plane for direct field visualization.

This combination of hyperbolic and ring mirrors is an innovation rarely implemented at a national level in RoboCupJunior Soccer Open, and represents a significant step forward in perception architecture for this category.

### Recommendations
For future iterations, it is recommended to optimize the mechanical alignment and calibration between both mirror systems to reduce geometric distortion, evaluate the integration of data fusion techniques between the omnidirectional and forward-facing cameras to improve real-time decision-making, and explore more compact mirror mounting solutions to reduce the overall height profile of the robot.

## Line Detection System PCB Layout
The line detection system was implemented across four independent PCBs arranged in a combined circular and X-shaped configuration of LEDs and phototransistors surrounding the robot. This layout was selected based on configurations used by high-level international RoboCupJunior teams, as it guarantees complete and precise detection of field boundary lines from any angle and direction of movement, eliminating coverage blind spots.

### Recommendations
For future iterations, it is recommended to migrate from the PT331C phototransistor to the TEPT5700 model, which offers a lower-profile package better suited for ground-level operation, greater sensitivity to reflected light, and improved mechanical durability under competition conditions.

## Resources
You can access all mechanical files used in the development by following this drive link:
https://drive.google.com/drive/folders/1iLI8nZqHYHWCFtoLrhFHCsU3mt8n4tq5?usp=sharing
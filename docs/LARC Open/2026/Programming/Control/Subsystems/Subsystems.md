# Subsystems Overview

The LARC Open 2026 robot software is organized into independent subsystems. This
structure makes the code easier to test, debug, and maintain, since each subsystem
is responsible for a specific part of the robot behavior.

The main subsystems are:

- Drive
- OmniKinematics
- Odometry
- DCMotors
- Elevator
- Servos
- Vision
- Controllers

Sensors are documented separately in the `Sensors` section.

---

## Drive

The `Drive` subsystem is responsible for controlling the robot movement. It
receives movement commands such as forward, backward, left, right, stop, and
translation commands, and converts them into motor outputs.

For LARC Open 2026, the robot uses an omnidirectional X-base with four omni
wheels placed at 45-degree angles. This was an important improvement compared to
the LARC Open 2025 robot, which used mecanum wheels.

The X-shaped omniwheel base allows the robot to move in any direction while
maintaining its orientation, which is especially useful for obstacle avoidance,
line correction, and precise alignment during the autonomous routine.

The movement system was integrated with odometry-based control to improve movement
precision during autonomous navigation.

---

## OmniKinematics

The `OmniKinematics` subsystem calculates the required wheel speeds based on the
desired robot movement.

The robot movement is represented using:

- `vx`: lateral movement
- `vy`: forward/backward movement
- `omega`: rotational movement

These values are converted into individual wheel commands for the four omni
wheels. This allows the robot to combine translation and rotation when needed.

In the state machine, this is especially useful during line-following and corner
alignment, where the robot needs to move forward while applying small lateral
corrections.

---

## Odometry

The odometry subsystem estimates the robot position using encoder feedback and
orientation information.

It is used to track:

- X position
- Y position
- Robot heading
- Wheel RPMs

The state machine uses odometry-based movement commands such as:

- `forward()`
- `backward()`
- `left()`
- `right()`
- `setTranslation()`
- `stop()`

This allows the robot to move in a more controlled way than using raw motor
commands directly.

Odometry was also used to improve stability during autonomous navigation,
especially in states such as `START`, `POOL`, `LOOKFORLINE`, `LOOKFORCORNER`,
`BEANS`, and `BEANSGOBACK`.

---

## DCMotors

The `DCMotors` subsystem controls the individual DC motors of the robot.

Each motor receives speed and direction commands from the higher-level movement
systems. This separation allows the robot to keep low-level motor control
independent from the state machine logic.

This makes it easier to test each motor individually and to modify the movement
logic without changing the motor driver implementation.

---

## Elevator

The elevator subsystem controls the vertical mechanism used during the autonomous
routine.

For the TMR configuration, the elevator startup routine was optimized to save time.
Instead of always moving down until the limit switch was reached, the elevator
automatically moved upward at the beginning of the run.

If the button/limit switch was pressed during this movement, the elevator moved
down only when necessary. This avoided wasting time running a full downward
initialization sequence when the elevator was already in the correct lower
position.

This behavior allowed the robot to start the routine faster while still keeping a
safe reference through the limit switch.

In the state machine, the elevator is mainly used during:

- `START`
- `BEANSGOBACK`

---

## Servos

The servo subsystem controls the intake mechanisms of the robot.

The intakes are used during the bean collection routine and can be deployed or
returned to their home position depending on the current autonomous behavior.

During the final weeks before TMR, the servo power configuration was modified due
to voltage drops that occurred when using the butterfly board inherited from the
LARC Open 2025 robot.

These voltage drops affected the stability of the intake servos during operation.
To improve reliability, the servos were connected directly to the main processing
board in use, the Raspberry Pi 5, instead of the previous shared configuration.

This modification reduced instability during the autonomous routine and improved
the consistency of the intake system during bean collection.

Although the TMR configuration mainly used the routine up to `BEANS` and
`BEANSGOBACK`, the servo subsystem remains part of the complete architecture for
the full LARC Open routine.

---

## Vision

The vision subsystem communicates with the external vision system used for bean
detection.

In the state machine, vision is started before the bean collection routine and
stopped during states where it is not needed. It also includes error handling, so
if a critical vision error occurs, the robot can stop safely.

Vision is mainly used during:

- `LOOKFORCORNER`
- `BEANS`
- `BEANSGOBACK`

The vision system communicates with the robot controller through serial
communication, allowing the embedded system to react to bean detections in real
time.

---

## Controllers

The controllers subsystem contains reusable control logic, such as PID
controllers.

These controllers are used to improve movement stability, line alignment, and
closed-loop behavior. For example, QTR line position is used to generate a
correction value that helps the robot stay aligned while moving.

The control system also integrates odometry and orientation stabilization to
improve autonomous navigation consistency.

---

## TMR Configuration

For TMR, the state machine was used only up to the `BEANS` routine. In
`BEANSGOBACK`, the robot remained positioned at the corner to finish the routine
safely instead of continuing to the full return and benefits sequence.

Because of this, the following states were not used during the TMR configuration:

- `POOLSGOBACK`
- `LOOKFORLINEBACKWARDS`
- `BENEFITSSTARTCORNER`
- `BENEFITS`

These states remain part of the complete autonomous architecture and were designed
for the full LARC Open routine, including the return navigation and benefits
section.

This modular structure allowed the robot to use only the required portion of the
autonomous routine depending on the competition configuration.
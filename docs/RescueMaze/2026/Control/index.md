# Control overview

Robot control is the system in charge of moving (and controlling) the robot as expected.
This system is of relevance in Rescue Maze as it helps the robot navigate through obstacles,
locate itself, and respond to environmental cues, which is fundamental for exploring the
whole maze using the algorithm.

## Sensors
One of the most important elements needed to develop a stable control is the use of reliable
sensors which give feedback to the robot about its current state. The sensor input is then
used to move the robot accordingly and reach the target positions.

See [Sensors](Sensors/index.md) for information about the sensors used.

## PID

> The PID controller is a control loop feedback mechanism widely used in industrial control systems.
A PID controller continuously calculates an error value as the difference between a desired setpoint
and a measured process variable and applies a correction based on proportional, integral, and derivative
terms (sometimes denoted P, I, and D respectively) which give their name to the controller type.

In Rescue Maze, PID controllers are used to make the robot move straight, rotate to desired angles,
and navigate ramps stably. The PID control regulates the PWM signal sent to each motor so that they
approach a target speed. Angle error from the BNO055 IMU is fed into a heading PID whose output
is added or subtracted from each side's speed reference, keeping the robot on course. Separate PID
instances handle ramp climbing and wall-centering using lateral VLX readings.

See [PID](PID.md) for information about the PID implementation.

## Code

The main class of control is the `motors` class. The algorithm only uses these functions directly:

    To go forward one tile:
        - void ahead();

    To rotate to an absolute heading:
        - void rotate(float);

    To dispense a rescue kit and perform the victim LED sequence:
        - void harmedVictim();
        - void stableVictim();
        - void unharmedVictim();

    To check whether a wall is present in a given direction:
        - bool isWall(uint8_t);

    To handle checkpoint tile logic:
        - void checkpointElection();

    Global state variables read by the algorithm:
        - bool blackTile      — robot reversed off a black (void) tile
        - bool blueTile       — robot stopped on a blue (evacuation) tile
        - bool checkpoint     — robot is on a checkpoint tile
        - uint8_t victim      — victim type detected
        - uint8_t kitState    — which side the rescue kit is on
        - bool buttonPressed  — emergency stop / start signal
        - uint8_t rampState   — current ramp phase (0 = flat, 1 = ascending, 2 = descending)

All remaining functions are handled internally. For example, `ahead()` automatically:
- Reads the TCS34725 color sensor and reacts to black tiles or waits on blue tiles
- Reads limit switches and executes a collision-recovery routine
- Uses encoder pulses and BNO055 heading to run PID and keep the robot straight
- Detects and traverses ramps via pitch readings from the IMU
- Uses front and back VLX ToF sensors as a dual fallback for tile distance estimation,
  switching to encoder-only mode if both sensors fall out of valid range

## Robustness

As more behaviors were added, the robot began to fail in unexpected ways. To address this,
overall robustness was improved by adding ceilings to PID outputs, enforcing hard `constrain()`
limits on motor commands, introducing sensor validity checks before acting on readings,
and refining reading logic to filter out-of-range values. A 5-second `millis()`-based timeout
was also added to `ahead()` and `moveDistance()` so that encoder failures can no longer cause
the robot to hang indefinitely.

See [PID](PID.md) and [Sensors](Sensors/index.md) for more details.

## Debug and calibration
For debugging and calibration, an OLED screen displays sensor detections in real time.
A dedicated test class provides functions to exercise all sensors and PID loops individually.
It also includes a `calibrateColors()` function that walks through each tile type and prints
the raw RGB readings, making it straightforward to update calibration thresholds before a run.

## Problems

### Ramp
Relocating the compute board to a Raspberry Pi 5 shifted the center of mass forward,
making the robot unable to climb ramps reliably. Climb speed was lowered and tuned
alongside mechanical adjustments to the weight distribution. The result is a controlled,
stable ascent rather than a slower one — the robot no longer stalls or tips on the way up.

### Wall climbing
Because the wheels were uncovered, the robot would occasionally ride up against back walls
instead of stopping flush. Movement routines were updated to avoid approaching back walls
closely, and additional safety checks were added inside `ahead()` to detect and handle
this condition before it becomes a collision.

### Color detection
The TCS34725 color sensor was mounted too high and lacked shielding from ambient light,
causing inconsistent readings across runs. A dedicated LED was added directly next to the
sensor for consistent illumination, and the sensor's timing budget was increased to allow
longer integration, producing more distinct RGB values between tile colors.

### ToF sensor reliability
The VLX53L0X ToF sensors were not rigidly mounted, causing them to shift slightly and
return erratic distances. As a software fix, `ahead()` now implements a dual-sensor fallback:
it prefers the front sensor pair, but automatically switches to the rear sensor — and then
to encoder-only mode — if either reading falls outside a valid range.
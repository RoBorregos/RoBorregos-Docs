# State Machine

The autonomous behavior of the LARC Open robot is controlled by a modular state
machine. Each state represents a specific task of the run, such as initialization,
pool navigation, line search, bean collection, returning, and final benefit-area
navigation.

The state machine allows the robot to react to sensors in real time while keeping
the code organized and easier to debug. Every transition resets important timers,
action stages, vision guards, and correction flags to avoid carrying behavior from
one state into another.

## Main States

The robot uses the following main states:

- `START`
- `POOL`
- `LOOKFORLINE`
- `LOOKFORCORNER`
- `BEANS`
- `BEANSGOBACK`
- `POOLSGOBACK`
- `LOOKFORLINEBACKWARDS`
- `BENEFITSSTARTCORNER`
- `BENEFITS`
- `STOP`

Each state is handled by a specific function, which makes the autonomous routine
more modular and easier to modify.

## Sensor Updates

At the beginning of every update cycle, the robot refreshes all main systems:

- Drive system
- IR sensors
- Ultrasonic sensors
- QTR line sensor
- Vision system
- ToF sensors

This ensures that every state uses the most recent sensor information before
making movement or transition decisions.

## START

The `START` state prepares the robot before entering the main navigation routine.

During this state, the robot:

1. Stops the vision system.
2. Checks the limit switch.
3. Runs the elevator initialization sequence.
4. Waits for the required startup delays.
5. Moves forward while temporarily ignoring the IR sensors.
6. Transitions into `POOL`.

The IR sensors are ignored for a short time at the beginning to prevent the robot
from reacting to the starting line or initial field conditions too early.

## POOL

The `POOL` state is the main forward navigation state. It allows the robot to move
through the pool section while avoiding obstacles and correcting its position if a
line is detected on either side.

This state uses three substates:

- `FORWARD`
- `AVOID_LEFT`
- `AVOID_RIGHT`

### FORWARD

In `FORWARD`, the robot moves forward while monitoring obstacle detection and side
line detection.

If an obstacle is detected, the robot changes to `AVOID_LEFT` by default.

If a line is detected on the right side, the robot performs a short correction to
the left. If a line is detected on the left side, it corrects to the right.

If no obstacle is detected for a defined amount of time, the robot transitions to
`LOOKFORLINE`.

### AVOID_LEFT

In `AVOID_LEFT`, the robot moves left to avoid an obstacle.

If the left side detects a line for enough time, the robot changes direction and
switches to `AVOID_RIGHT`.

If the obstacle disappears and remains clear for a delay, the robot returns to
`FORWARD`.

### AVOID_RIGHT

In `AVOID_RIGHT`, the robot moves right to avoid an obstacle.

If the right side detects a line for enough time, the robot switches back to
`AVOID_LEFT`.

If the obstacle disappears and remains clear for a delay, the robot returns to
`FORWARD`.

## LOOKFORLINE

The `LOOKFORLINE` state searches for the line that guides the robot toward the
collection zone.

This state uses:

- Front IR sensors to detect the line
- QTR sensor to confirm that the robot is on the line
- Ultrasonic sensors to slow down near the tree or obstacle
- Lateral correction when only one side detects the line

If the front sensors detect the line and the QTR confirms the robot is on it, the
robot stops and transitions to `LOOKFORCORNER`.

When only one side detects the line, the robot performs a short lateral correction
to re-center itself.

## LOOKFORCORNER

The `LOOKFORCORNER` state aligns the robot with the corner before starting the bean
collection routine.

The robot moves using QTR-based lateral correction and forward motion until the
left corner sensor detects the target corner.

Once the corner is detected, the robot:

1. Brakes.
2. Deploys the intake servos.
3. Waits briefly.
4. Soft-starts movement again.
5. Transitions to `BEANS`.

## BEANS

The `BEANS` state is the main collection routine.

During this state, the vision system is enabled to detect beans. Depending on the
vision result, the upper and lower intake servos are deployed or returned home.

The robot follows the line using QTR correction while moving through the collection
area.

If the line is lost for too long, the robot backs up and returns to `POOL` as a
recovery strategy.

If the right corner is detected, the robot stops the collection behavior and
transitions to `BEANSGOBACK`.

## BEANSGOBACK

The `BEANSGOBACK` state moves the robot back after the bean collection routine.

The elevator is first lowered until the limit switch condition is reached. Then the
robot follows the line in the opposite direction while still controlling the intake
servos.

When the left corner is detected, the robot brakes and transitions to
`POOLSGOBACK`.

## POOLSGOBACK

The `POOLSGOBACK` state applies similar obstacle-avoidance logic as `POOL`, but
while moving backward.

It also uses the same pool substates:

- `FORWARD`
- `AVOID_LEFT`
- `AVOID_RIGHT`

If no obstacle is detected for a defined amount of time, the robot transitions to
`LOOKFORLINEBACKWARDS`.

## LOOKFORLINEBACKWARDS

The `LOOKFORLINEBACKWARDS` state searches for the rear line while the robot is
moving backward.

If only one rear sensor detects the line, the robot performs a short lateral
correction. Once the rear line is detected properly, the robot transitions to
`BENEFITSSTARTCORNER`.

## BENEFITSSTARTCORNER

The `BENEFITSSTARTCORNER` state aligns the robot before entering the benefits
section.

The robot follows the line using QTR correction until the left corner is detected.
After braking and waiting, it transitions to `BENEFITS`.

## BENEFITS

The `BENEFITS` state controls the final movement routine after the benefits area
has been reached.

The robot continues moving with line correction until the right corner is detected.
After a final stop delay, the robot transitions to `STOP`.

## STOP

The `STOP` state is the final safe state.

The robot brakes and stops all movement.

## Recovery and Stability Strategies

Several mechanisms are included to improve reliability:

- Obstacle latching to avoid reacting to noisy ultrasonic readings.
- Clear-delay timers before returning to forward movement.
- Minimum avoid time before changing avoidance direction.
- Side-detection hold time to prevent false transitions.
- QTR-based line confirmation before entering corner states.
- Vision critical-error detection during bean collection.
- Lost-line timeout recovery in `BEANS`.
- Soft-start ramps to reduce sudden movement.

These mechanisms prevent unstable transitions and help the robot recover when a
sensor reading is noisy or when the robot loses alignment.
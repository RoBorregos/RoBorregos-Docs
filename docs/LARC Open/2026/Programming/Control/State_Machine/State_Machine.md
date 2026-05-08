# State Machine

The autonomous behavior of the LARC Open robot is controlled by a modular state
machine. Each state represents a specific part of the run, including initialization,
pool navigation, line search, bean collection, return behavior, and final stopping.

The state machine was designed to keep the autonomous logic organized and easier
to debug. Each state has its own handler function, and transitions reset important
timers, action stages, line correction flags, and vision guards.

## Main States

The robot uses the following states:

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

## General Update Cycle

At the beginning of every loop, the state machine updates the main subsystems:

- IR sensors
- Front QTR sensor
- Vision system
- Left and right ToF sensors
- Odometry movement system

The robot also calculates the QTR line position and generates a lateral correction
value. This correction is later used in line-following states to keep the robot
aligned with the path.

## Competition Configuration for TMR 2026

For the Torneo Mexicano de Robótica (TMR) 2026, the robot operated using the
state machine only up to the `BEANS` and `BEANSGOBACK` states.

During the competition configuration, once the robot completed the bean collection
routine and reached the corner inside `BEANSGOBACK`, it remained positioned in
that area to finalize the routine safely instead of continuing to the remaining
states of the full autonomous sequence.

Because of this, the following states were not used during the TMR configuration:

- `POOLSGOBACK`
- `LOOKFORLINEBACKWARDS`
- `BENEFITSSTARTCORNER`
- `BENEFITS`

These states remain part of the complete autonomous architecture and were designed
for the full LARC Open routine, including the return navigation and benefits
section. However, for TMR the priority was maximizing stability and reliability
during the bean collection phase.

This modular structure allowed the robot to use only the required portion of the
autonomous routine depending on the competition configuration.

## START

The `START` state prepares the robot before entering the main navigation routine.

During this state, the robot:

1. Stops movement.
2. Reads the limit switch.
3. Moves the elevator according to the limit switch condition.
4. Waits for the elevator sequence to finish.
5. Moves forward using odometry-based movement.
6. Ignores IR readings for a short startup period.
7. Transitions to `POOL`.

This prevents the robot from reacting too early to the starting line or initial
arena conditions.

## POOL

The `POOL` state is the main navigation state. The robot moves forward while
detecting obstacles with the ToF sensors and checking the IR sensors for side
lines.

This state uses three substates:

- `FORWARD`
- `AVOID_LEFT`
- `AVOID_RIGHT`

### FORWARD

In `FORWARD`, the robot moves forward using odometry movement.

If an obstacle is detected, the robot switches to `AVOID_LEFT` by default.

If the robot detects a line on the right side, it performs a short correction to
the left. If it detects a line on the left side, it corrects to the right.

If no obstacle is detected for a defined amount of time, the robot transitions to
`LOOKFORLINE`.

### AVOID_LEFT

In `AVOID_LEFT`, the robot moves left to avoid an obstacle.

If the obstacle is too close, the robot backs up as a safety behavior. If the left
side detects a line, the robot switches to `AVOID_RIGHT`.

When the obstacle disappears and remains clear for a delay, the robot returns to
`FORWARD`.

### AVOID_RIGHT

In `AVOID_RIGHT`, the robot moves right to avoid an obstacle.

If the obstacle is too close, the robot backs up. If the right side detects a line,
the robot switches to `AVOID_LEFT`.

When the obstacle disappears and remains clear for a delay, the robot returns to
`FORWARD`.

## LOOKFORLINE

The `LOOKFORLINE` state searches for the line that guides the robot toward the
collection area.

Before starting the normal line search, the robot performs a short movement
sequence to reposition itself. After that, it uses the front IR sensors, the QTR
sensor, and the ToF sensors to determine whether it is detecting a real border or
the target line.

If only one side detects a border, the robot performs a short lateral correction.

When the front IR sensors detect the line and the QTR confirms that the robot is
on the line, the robot stops and transitions to `LOOKFORCORNER`.

## LOOKFORCORNER

The `LOOKFORCORNER` state aligns the robot with the collection corner.

The robot uses the QTR position to calculate a correction and moves while keeping
itself aligned with the line. When the left rear/corner sensor detects the corner,
the robot stops and starts the vision system for bean detection.

After a waiting period, the robot transitions to `BEANS`.

## BEANS

The `BEANS` state is the main bean collection routine.

In this state, the robot follows the line using QTR-based correction while moving
through the collection area. The vision system is monitored for critical errors.

If the line is lost, the robot moves backward to try to recover. If the line is
lost for too long, the robot exits the collection routine and transitions to
`POOLSGOBACK`.

If the right corner is detected, the robot stops and transitions to
`BEANSGOBACK`.

## BEANSGOBACK

The `BEANSGOBACK` state handles the return behavior after the bean collection
routine.

The robot stops the vision system, clears vision errors, and prepares the elevator.
Then it follows the line in the opposite direction using QTR correction.

When the front-left corner is detected, the robot stops. After a waiting period,
the vision system is started again and the robot transitions back to `BEANS`.

## POOLSGOBACK

The `POOLSGOBACK` state applies pool navigation logic while the robot is returning.

It uses the same pool substates:

- `FORWARD`
- `AVOID_LEFT`
- `AVOID_RIGHT`

If an obstacle is detected, the robot avoids it by moving left or right. If no
obstacle is detected for a defined time, it transitions to `LOOKFORLINEBACKWARDS`.

## LOOKFORLINEBACKWARDS

The `LOOKFORLINEBACKWARDS` state searches for the rear line while moving backward.

If only one rear sensor detects the line, the robot performs a short lateral
correction. Once the rear line is detected, the robot transitions to
`BENEFITSSTARTCORNER`.

## BENEFITSSTARTCORNER

The `BENEFITSSTARTCORNER` state aligns the robot before entering the benefits
section.

The robot follows the line until the left corner is detected. After braking and
waiting, it transitions to `BENEFITS`.

## BENEFITS

The `BENEFITS` state controls the final movement routine.

The robot moves while applying line correction until the right corner is detected.
After a final stop delay, it transitions to `STOP`.

## STOP

The `STOP` state is the final safe state.

The robot brakes and stops all movement.

## Reliability Strategies

The state machine includes several reliability mechanisms:

- ToF warmup time before using distance readings.
- Obstacle latching to avoid false detections from noisy sensors.
- Obstacle release delay before returning to normal movement.
- Side line correction during pool navigation.
- Too-close obstacle backup behavior.
- QTR-based correction for line alignment.
- Lost-line recovery during bean collection.
- Vision critical-error detection.
- Timed action stages for controlled transitions.
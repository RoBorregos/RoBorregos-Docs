# PID

PID control was used to reach the requested RPMs for the motors. To reach the requested
RPM, the behavior of the constants was plotted, in this way it was possible to observe how
much it oscillated until reaching this, so after many modifications in these variables, it
was possible to reach the desired behavior. The constants are kP, kI and kD, which indicate
the relevance of the error in the proportional, integral and derivative.

This control has as reference the speed calculated with the ticks or interruptions of the
encoders, for this it was checked that each motor had the same amount of ticks per revolution.

## How does it work?

A magnetic encoder works by reading a small magnet attached to the motor shaft using a
Hall-effect sensor. As the shaft rotates, the sensor detects the changing magnetic field
and generates pulses that are counted using Arduino's `attachInterrupt()` function. The
number of pulses per unit time gives the wheel speed, which is then used as the measured
variable for the speed PID.

`attachInterrupt()` function reference:
[Arduino Reference](https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/)

In this way the speed of each motor is taken out so that the PID control allows to increase
the pwm variable to reach the desired speed so that all motors go at the same speed.

A fixed `delay(20)` that previously blocked the speed control loop was removed, allowing
the PID to update at a smoother and more consistent rate.

## PID Class

As more behaviors were added, the PID class required several fixes to prevent unstable
or runaway outputs.

**Anti-windup** — The integral term is now clamped to a maximum limit. Without this, the
integrator would build up during a stall and cause a sudden overcorrection once the robot
started moving again.

**Safe initialization** — All internal values are explicitly set to zero on startup,
avoiding random behavior caused by uninitialized variables.

**Integral reset on constant change** — When PID constants are updated, the accumulated
integral is cleared. Keeping the old value would cause an unwanted output spike at the
moment of the change.

**Delta-time guard** — The PID returns zero if called too frequently, preventing
division-by-zero errors in the derivative calculation.

**External reset** — A `resetIntegral()` method allows other parts of the code to clear
the integrator at known points, such as the start of a new movement.

Impementation of PID:
[here](https://github.com/RoBorregos/robocup-rescuemaze-2026/blob/TMR-final/src/PID.cpp)

## PID Orientation

To make the robot go in a specific orientation, the control was implemented with reference
to the gyroscope (BNO), so that also for this one the constants had to be modified to reach
as soon as possible its setpoint without much oscillation. For this the actual angle was
used, i.e. the reference of the BNO and the desired angle if it was loaded more to one side
the speed of the motors on the same side increased to reach their respective angle.

## Control speed respect target distance

The robot must move forward in a controlled manner, and sudden braking should be avoided
so that it stops at the desired position. For this reason, as it approaches the target
distance, its speed should gradually decrease. To achieve this, the map function is used
by inputting the minimum and maximum speeds relative to the initial and final distances.

## Dual-Sensor Fallback in Forward Movement

When moving forward, the robot first uses the front VLX sensors for distance measurement.
If those give an invalid reading it switches to the rear VLX sensor, and if that also
fails it falls back to encoder ticks alone. This keeps the robot moving reliably even if
one sensor loses its reading mid-movement.

A 5-second timeout was also added to `ahead()` and `moveDistance()` — if a move takes
longer than expected the robot stops and resets instead of getting stuck indefinitely.

## PID Diagram

![PID_Diagram](/assets/maze/PID.png)

## PID Walls

In certain scenarios as would be the case of the ramp there are always walls on its sides
so it was devised to apply a control with respect to the distances of the sides that is to
say that if it was told to keep the same distance to both walls, this oscillated to reach
that point and leave the ramp, of course the counters had to be modified to reach the
desired behavior as well as the desired distance.
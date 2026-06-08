# PID

## Orientation Control — PID Controller
To maintain accurate heading during movement, the robot implements a PID (Proportional-Integral-Derivative) controller. A PID controller continuously calculates a correction output based on the difference between a desired value and the current measured value — known as the error. In this case, the error represents the angular deviation between the robot's target heading and its actual orientation as measured by the IMU.

The controller combines three terms to produce its output. The proportional term generates an immediate correction proportional to the current error. The integral term accumulates error over time to eliminate persistent steady-state offsets. The derivative term anticipates future error by reacting to the rate of change, damping oscillations and improving stability.

## Implementation Details

Several engineering decisions were made to improve the robustness of the controller beyond a standard PID implementation.

The controller enforces a minimum time step of 20 ms between updates, ensuring a consistent sampling rate and preventing numerical instability caused by near-zero delta time values. If an invalid time step is detected, the controller returns the last valid output rather than producing an erroneous correction.

The integral term is computed using the trapezoidal rule, which averages the current and previous error before accumulating, providing a more accurate numerical integration compared to a simple rectangular approximation.

To prevent integral windup — a condition where the integral term grows unboundedly when the system is saturated — the controller implements a conditional accumulation strategy. The integral is only updated when the absolute error exceeds a deadband of 1.5°, filtering out negligible corrections near the setpoint. Furthermore, a prospective saturation check is performed before each accumulation: if the new integral increment would push the output beyond the defined maximum, the update is only allowed if the error is actively working to reduce the saturation, ensuring the integral never accumulates in a direction that worsens the response.

The final output is clamped to a configurable maximum value, and the integral accumulator is independently clamped to a separate maximum to provide an additional layer of windup protection.

## Recommendations
For future iterations, it is recommended to evaluate the addition of a derivative low-pass filter to reduce sensitivity to high-frequency noise in the error signal, implement an auto-tuning routine to streamline PID gain calibration during competition setup, and log integral and derivative values in real time during testing to facilitate more systematic tuning of the controller parameters.
# Ball Detection – IR Ring
Ball tracking is managed by a ring of 12 infrared (IR) receivers arranged in a circular configuration and connected to a dedicated ATmega328P microcontroller. This system allows the robot to detect the angle and strength of the infrared signal emitted by the ball, which typically contains IR LEDs. The ATmega328P processes the signals from the IR sensors and communicates the calculated ball angle and signal strength to the main controller via UART at a baud rate of 115200.

The angle is mapped in the range of -180° to 180°, where 0° represents the front of the robot, negative values indicate the ball is to the left, and positive values to the right. This angular data is used to orient the robot towards the ball with high precision.

To improve the stability of the readings, the system uses the SingleEMAFilterLib library, which applies an Exponential Moving Average (EMA) filter. This smooths out noise and sudden changes in the sensor data.

### Control Logic Overview
The main logic is implemented in the IRRing class. The most important methods include:

- **UpdateData()**
Reads UART input and updates the angle (a) and strength (r) values.

- **GetAngle()**
Applies correction factors depending on the angle magnitude:

    - Far angles (behind): multiplied by ballFollowOffsetBack

    - Side angles: multiplied by ballFollowOffsetSide

    - Near angles (in front): multiplied by ballFollowOffsetFront

It then filters the result to avoid sudden spikes and returns a smoothed angle (negated to match motor convention).

- **GetStrength()**
Returns the smoothed signal strength.

We use the EMA filter like this:

```cpp
SingleEMAFilter<double> filterAngle(0.6);
SingleEMAFilter<double> filterStr(0.6);
```

Each time a new value is received:

```cpp
filterAngle.AddValue(angle);
filterStr.AddValue(strength);
```
This smooths the data by giving 60% weight to the new value and 40% to the previous result.

The SingleEMAFilterLib was developed by Luis Llamas. It applies a single-pole exponential moving average, a common digital filter in robotics and signal processing.

**Key Concepts**
- Low-pass filter:Keeps slow changes, removes fast noise.

- High-pass filter: Keeps fast changes, removes slow trends (optional in this case).

- Alpha (α): A value between 0 and 1 that defines how reactive the filter is.

**Core Equations**
```cpp
lowPass = α * new_value + (1 - α) * old_filtered_value;
```
Higher α means more reactive but less stable output.

**Main Functions**
- **AddValue(value)**: Applies filtering and updates internal state.

- **GetLowPass()**: Returns the filtered (smoothed) value.

- **GetHighPass()**: Returns the fast-changing component (not used here).

```cpp
SingleEMAFilter<float> filter(0.6);
float filteredValue = filter.AddValue(rawValue);
```

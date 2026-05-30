## Phototransistor and MUX Library

The phototransistor system is used to detect the white field lines so the robot can avoid leaving the field or crossing penalty lines. This is especially important because a line mistake can cause a lack of progress penalty and gives the opponent time to recover.

Each side of the robot has an array of phototransistors on the lower PCBs. Because there are several sensors and only a limited number of analog inputs, we use multiplexers to select which phototransistor channel is being read at each moment.

## Main Improvement

The biggest improvement was removing the need for fixed absolute thresholds.

Before this change, we had to manually tune constant threshold values every time the robot was tested in a different place. Lighting conditions, floor color, shadows, and LED intensity could all change the raw analog values, so a threshold that worked in one environment could fail in another.

Now, when the robot starts, it immediately samples the floor while it is on green. These values become the green baseline for each phototransistor channel. During the match, the robot compares the current value against that baseline.

Instead of asking "is this reading greater than a fixed line value?", the code asks:

```cpp
current_reading > green_baseline + line_margin
```

This means the robot detects a white line when the reading increases by more than the hardcoded margin over the green baseline.

## Why the Baseline Method Works

The useful observation is that, even when the absolute lighting changes, the difference between green floor and white line is usually very similar. The raw value for green may be higher or lower depending on the room, but the jump from green to white stays consistent enough to detect with the same margin.

This lets us keep a precise hardcoded margin while avoiding manual calibration every time the robot boots in a new place. The robot calibrates itself to the current green floor value, then uses the same increase margin to detect white.

## Code Behavior

At startup, the robot captures several samples from the phototransistors while it is on green floor. These samples are used to calculate the baseline values.

During play, each channel is read through the multiplexer. If any enabled channel rises above its baseline by more than the configured margin, the robot considers that side to be detecting a line.

This made the line detection much more practical because we no longer needed to constantly replace threshold constants before tests. The code could immediately adapt to the current floor conditions and detect changes as soon as the robot reached a line.

## Resistor and Noise Problems

The code worked well for detecting value changes, but the hardware made the readings noisier than expected.

The resistors on the phototransistor PCBs affected how quickly the circuit filtered voltage after detecting reflected light. Some resistor choices made the signal slow to return to the green baseline after the sensor saw a white line. This created noise and delayed recovery, which made the phototransistors less reliable than expected.

LED intensity also affected the readings. Brighter LEDs made the line easier to see, but they could also make the phototransistor signal harder to filter cleanly. Because of this, resistor values and LED behavior need to be tested together, not separately.

## Planned Auto-Recalibration

We also planned to add automatic recalibration during the match.

The idea was to recapture the green baseline every few seconds whenever the robot had not detected a line recently. For example, if the robot spends five seconds only seeing green, it can safely assume that the current floor readings are valid green values and update the baseline.

This would help compensate for slow lighting changes, battery effects, and sensor drift while keeping the same hardcoded margin for detecting the jump from green to white.

## Recommendations

Avoid spending match preparation time manually tuning raw threshold values. Automations like baseline capture make the robot faster to deploy and let the team focus on higher-impact work such as control, ball following, and strategy.

For future versions, we should:

- Keep the baseline-plus-margin method instead of returning to fixed absolute thresholds.
- Test resistor values on a breadboard before soldering the PCBs.
- Verify how quickly each phototransistor returns to its green baseline after seeing a white line.
- Tune the hardcoded line margin using the difference between green and white, not the raw green value.
- Implement periodic green recalibration only when no line has been detected for several seconds.
- Use debug prints to inspect baseline, current reading, and delta for each channel during testing.

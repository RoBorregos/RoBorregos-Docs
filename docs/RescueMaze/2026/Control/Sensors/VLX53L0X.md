# VL53L0X

The VL53L0X is a time of flight distance sensor. In Rescue Maze, it was used to detect
walls and measure robot displacement.

[Product link](https://www.adafruit.com/product/3317)
[Library used](https://github.com/adafruit/Adafruit_VL53L0X)

The sensor has multiple modes (see [Adafruit_VL53L0X.cpp](https://github.com/adafruit/Adafruit_VL53L0X/blob/master/src/Adafruit_VL53L0X.cpp) lines 210-219).

## Considerations

While this sensor gives accurate readings at close range, reliability decreases with
distance. With the timing budget used in this project (`50000 µs`), the sensor was able to reach up to ~3 meters, but usable and consistent readings are only reliable
at much shorter distances. If you need to check whether something is far away, it is
possible, but always validate that the reading is within a sensible range and not being
filtered out as out-of-range before acting on it.

The black VL53L0X variant was tested and reached longer reliable distances than the blue
one, it is recommended if longer range is needed.

## VLX Class

The `VLX` class wraps the Adafruit library and adds continuous measurement mode, reading
validation, and a smoothing filter to make distance readings more stable and reliable.

**Continuous mode** — The sensor is started with `startRangeContinuous(100)` after
initialization. This tells the sensor to keep measuring on its own at 100ms intervals
rather than triggering a new measurement each time a reading is requested. This reduces
latency and avoids blocking the main loop while waiting for a result.

**Timing budget** — The timing budget is set to `50000 µs` (50 ms). A longer budget
allows the sensor more time to average its measurement internally, producing more stable
and consistent readings, especially at longer distances. It is worth considering that the minimal precise timing budget recommended by the datasheet is `33000 µs` (33 ms). During testing the timing budget selected performed better than the shorter suggested one.

**Reading validation and retry** — Each call to `getDistance()` checks whether the raw
reading is valid by inspecting the range status and whether a timeout occurred. If the
first read fails, a second attempt is made automatically before falling back to the
out-of-range value. This handles occasional I2C glitches without immediately discarding
the reading.

**Ring buffer filter with trimmed mean** — Valid readings are stored in a small ring
buffer. When enough samples are available, they are sorted and the outermost values are
trimmed before averaging. This rejects sudden spikes or dips caused by reflections or
sensor noise, giving a smoother and more representative distance value.

**Invalid read counter** — If the sensor returns out-of-range values several times in a
row (`kMaxInvalidReads = 2`), the filter buffer is fully cleared and the output is set to
the out-of-range sentinel value. This prevents the robot from acting on a stale last-known
distance when the sensor has genuinely lost its target, such as when facing an open
corridor.

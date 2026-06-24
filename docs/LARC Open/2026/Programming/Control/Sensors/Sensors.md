# Sensors Documentation - LARC Robot 2026

## Máximo Fajardo

All sensors live on the Teensy 4.1. Each one has its own library under `lib/`, all share a non-blocking `update()` pattern (no `delay()` in the loop), and tunable values live in `constants.h` so we never dig into library internals to tweak things.

---

### `ToF` — VL53L0X / VL53L1X (I2C)

Time-of-flight distance sensors. We use both VL53L0X and VL53L1X depending on what we have available at the moment, so the library takes a `ToFType` enum and dispatches to the right driver internally; same API for both.

The big thing is the **mux variant**. Every VL53 ships with the same factory I2C address (`0x29`), so to use more than one on the same bus you either reflash addresses (fragile, redo every boot) or use a TCA9548A I2C mux. We went with the mux. The constructor takes a `muxChannel` and a `TCA9548A` reference, and every `update()` calls `selectIfMux()` first to switch to the right channel.

`update()` is non-blocking and rate-limited so we don't poll faster than the sensor produces data. Bad readings (`0`, `65535`, out-of-range) are filtered out and we **keep the last known good value** instead of writing garbage. Hard-learned lesson: continuous mode + long mode + mux switching causes random `65535` readings, so when sensors are multiplexed we drop continuous and use single-shot.

---

### `TCA9548A` — I2C multiplexer

Simple wrapper around the TCA. The interesting bit is `selectChannel()` caches the current channel and skips the I2C write if we're already on it — no point burning cycles re-selecting every loop.

It also has scanners: `scanAll()` walks all 8 channels and prints what's on each, `scanDirect()` ignores the mux and scans the bus. Both saved a lot of debug time.

---

### `Mux74HC4067` — analog multiplexer

Analog counterpart to the TCA — 16 channels into one signal pin. We use it to share a single Teensy ADC across all the IR sensors and the QTR arrays.

It's a thin wrapper: `select(ch)` writes the 4 select bits, `read(ch)` selects + waits 50 µs for the analog signal to settle + reads.

---

### `QTR` — 8-channel reflectance array

Two QTR-8A arrays, one front and one rear, both through the same `Mux74HC4067`. The constructor takes a `firstChannel` so each array knows where it starts on the mux: `QTR qtrFront(0, mux)` reads `C0..C7`, `QTR qtrRear(8, mux)` reads `C8..C15`.

`update()` does three things:

1. Reads the 8 raw ADC values through the mux.
2. Normalizes each one to `0..1000` based on per-sensor calibration min/max (so a sensor that maxes at 800 still reads as 1000).
3. Computes a weighted-average position from `0` (extreme left) to `7000` (extreme right). If no sensor is over threshold, it keeps the last position instead of jumping to zero.

Calibration can be done two ways: live with `calibrate(durationMs)`, where you sweep the array over the line and it captures min/max, or by loading a preset from `constants.h` via `useDefaultCalibration(profile)`. After live calibration, `printCalibration()` dumps the values in a copy-paste-ready format for `constants.h`.

---

### `IRLine` and `IR_mux` — two flavors of the same thing

The IR line sensors have **two libraries** with the same public API but different backends. The reason is that the wiring kept changing — sometimes the IRs went straight to Teensy analog pins, sometimes they had to go through the analog mux because we'd run out of ADC pins. Instead of hardcoding one option, I split the library:

- **`IRLine`** — direct `analogRead()` from Teensy pins. Constructor takes 4 pins.
- **`IR_mux`** — same logic but reads through `Mux74HC4067`. Constructor takes a mux reference and 4 channel numbers.

Both use the exact same processing pipeline, so the rest of the code doesn't care which one is plugged in.

The pipeline:

1. **Raw analog read** (direct or through mux).
2. **Simple threshold** — is the value over the configured threshold? With per-sensor inversion via `invertedMask`, since some sensors saw the line as a dip and others as a spike depending on mounting.
3. **Hysteresis** — two thresholds, `threshHigh` and `threshLow`. State only flips to "detected" when the reading crosses `threshHigh`, and back to "clear" only when it drops below `threshLow`. Kills the chatter when readings hover at the edge.
4. **Consensus debounce** — a candidate state has to show up for `kDebounceCount` consecutive reads before getting committed to `stableState`. `getState()` returns `stableState`, not the raw read.

Three layers of filtering sounds like a lot, but on a moving robot with vibration and changing ambient light, single-stage filtering had way too many false transitions.

---

### `Ultrasonic` — HC-SR04 (state machine, not implemented)

This one didn't make it into the final robot... we replaced ultrasonics with VL53L1X ToFs because of pin constraints. But the library is built and works.

The whole point is that it never blocks. The standard Arduino way to read an HC-SR04 is `pulseIn()`, which blocks the CPU for up to ~30 ms waiting for the echo. On a tight control loop that's a disaster; IMU reads, motor control, and serial parsing all stall while the ultrasonic does its thing.

Instead, the library uses a 4 state machine driven by `millis()` and `micros()`:

- **`Idle`** — waiting until the ping period (e.g. 50 ms) has passed since the last ping.
- **`Trig_High`** — trigger pin is HIGH; wait 10 µs and drop it.
- **`Wait_Echo_Up`** — trigger LOW, watching for the echo to go HIGH. If it doesn't go high before `kEchoTimeoutUs`, mark invalid and reset.
- **`Wait_Echo_Down`** — echo is HIGH, waiting for it to drop. Pulse width gives the distance: `(now - echoRiseUs) / 58.0` for cm.

Each state advances on a time check or a pin read, never a `delay()`. So `update()` returns immediately if the state machine isn't ready to advance, and the rest of the loop runs normally. The cost is a few microseconds per loop iteration to check the current state.

# Vision Documentation - LARC Robot 2026

## Máximo Fajardo

The vision system is split between the Raspberry Pi 5 and the Teensy 4.1. The Pi does all the heavy lifting: cameras, OpenCV, HSV masks, circle detection, and driving the servos. The Teensy just tells the Pi when to start and stop running scripts, and reads back the detection results to use in the state machine. They talk over USB serial with a small byte protocol.

---

### Architecture overview

The Pi runs three vision scripts, one per phase:

- `raspi_visao.py` — ZED stereo camera, splits the frame in half and detects beans on each side (top and bottom intake). This is the only vision script that ended up running on the final robot.
- `separator_visao.py` — Global Shutter camera, watches the separator chute for warm/cool balls. Built and tested working, not used in the final run.
- `benefits.py` — Logitech C920, detects colored boxes during the benefits phase. Built and tested working, not used in the final run.

Scripts don't talk to the Teensy directly. They print tagged lines (`VISION:XX`) to stdout, and a dispatcher process collects them and forwards everything over serial.

---

### `pi_dispatcher.py` — the boss process

The dispatcher runs as a systemd service on boot. It owns the serial port — only it writes to `/dev/ttyTeensy`, so two processes can't garble each other's bytes.

What it does:
1. Opens serial and sends `ACK_READY` (`0xB0`) to the Teensy.
2. Waits for a command (`START_BEANS`, `START_BENEFITS`, `STOP`, `STATUS`).
3. Launches the right vision scripts as subprocesses, captures their stdout in background threads.
4. Reads `VISION:XX` lines, merges the bits, and sends one byte per frame to the Teensy.
5. Health-checks the subprocesses every 2 seconds. If something dies, sends an error byte.

#### Bit merging

Vision data is packed into one byte where each bit is a detection flag:

```
bit 0 = beanTop    bit 1 = beanBottom
bit 2 = warmBall   bit 3 = coolBall
```

`raspi_visao` owns bits 0–1, `separator` owns bits 2–3. The dispatcher keeps a shared state byte and uses a mask so each script only writes to its own bits — that way one can't overwrite the other's flags.

One important rule: every script has to print `VISION:00` on empty frames too. If a script stays silent when nothing is detected, the bits get stuck high on the Teensy side.

#### Servo wakeup/sleep

The intake servos are driven by the Pi (GPIO 12 and 13). They have to be home when no script is running, otherwise they'd dump balls when the robot stops. The dispatcher handles this with `servo_wakeup.py`, a small short-lived subprocess that opens the pins, sweeps them, and releases them.

- On serial connect → sweep `up` (deploy).
- On `CMD_STOP`, disconnect, all-scripts-dead, or shutdown → sweep `down` (home).
- Phase-to-phase swaps skip the sweep, since the next phase grabs the pins right away.

A 200 ms `POST_KILL_SETTLE` delay runs before the down-sweep, but only if `raspi_visao` was running, since it's the script that holds GPIO 12/13. Without that delay, gpiozero hasn't fully released the pins yet and `servo_wakeup` can't claim them.

---

### `Vision.hpp` / `Vision.cpp` — Teensy-side interface

Teensy library that talks to the Pi. It wraps a `Stream&` (so it works on `Serial` or `Serial1`) and exposes simple getters: `beanTop()`, `beanBottom()`, `warmBall()`, `coolBall()`, plus box info for the benefits phase.

The Teensy's role here is small: it sends commands to start or stop scripts (`startBeans()`, `startBenefits()`, `stop()`), and reads back the detection bits the Pi forwards. It does not drive any servos itself, since on field tests, it was proven to be too unreliable/slow, and we didn´t exactly have a lot of time to debug that. So all the servos are all on the Pi.

#### Protocol

```
Teensy → Pi:
  0xA0 START BEANS    0xA1 STOP
  0xA2 STATUS         0xA3 START BENEFITS

Pi → Teensy:
  0xB0–0xB4 ACKs (ready, starting, running, stopped, benefits running)
  0x00–0x0F vision bitfield (single byte, no header)
  0xFE + 1 byte box type (red / blue)
  0xE0–0xEF + 1 byte error code
```

#### `update()` — the parser

The first version used inline `available() >= 2` checks for multi-byte messages, which broke all the time because USB serial doesn't guarantee both bytes arrive in the same `update()` call. The fix was a persistent state-machine parser.

When a header byte (`0xFE` or any error in `0xE0`–`0xEF`) comes in, `_parseState` flips to 1 and `_parseHeader` remembers which header is pending. The next byte is treated as the payload, even if it shows up several `update()` calls later. ACKs and the vision bitfield are single-byte, so they're handled inline.

#### Send guards and error flags

Each command has a `_xxxSent` flag so we don't spam duplicates. `resetGuards()` clears them on phase changes.

Errors are split: `_criticalError` triggers a robot stop, while `_separatorError` and `_benefitsError` are informational. `clearErrors()` runs on phase transitions so old errors don't trigger false stops.

---

### `raspi_visao.py` — bean detection (ZED stereo)

Main vision script. Reads frames from the ZED, splits each frame into left and right halves (one per intake), and runs detection on both independently.

#### Pipeline

For each half:

1. **Frame read** with `cap.set(CAP_PROP_BUFFERSIZE, 1)` to kill V4L2 latency. Without this, OpenCV buffers old frames and you get 200+ ms of lag.
2. **Downsample 2×** with `INTER_NEAREST`. The ZED gives 1344×376; full res is wasteful. Config values are stored at full resolution and auto-scaled at runtime (spatial ÷ 2, area ÷ 4).
3. **HSV conversion** of the half-frame.
4. **Two HSV masks**, one per bean color. HSV is more robust than RGB under changing lighting because hue stays roughly constant even when brightness shifts.
5. **Circle detection** with `HoughCircles` on the masked region. Hough fits circles to edges and gives back center and radius. The radius range is bounded so noise doesn't get picked up.
6. **Decision logic** — if a circle shows up in a valid region, set the bit (top or bottom) and print `VISION:XX`.

#### HSV masks

Each mask is a `cv2.inRange(hsv, lower, upper)`. Some colors (like red) wrap around the hue circle, so they need two masks `OR`-ed together. Lower and upper bounds come from the calibration JSON.

#### Pi-side servo control

`raspi_visao` drives the intake servos directly through gpiozero (pins 12/13). Deploy and home calls only fire on **state transitions** — once when a bean is first detected, once when it's first cleared — never every frame. Writing PWM every loop is what causes servo jitter, so this is the fix. There's also a debounce that requires N consecutive frames of the same reading before acting, which kills false positives.

After moving, the signal is detached after 400 ms via a timer so the servo stops drawing current and stops twitching.

#### Calibration tool

Tuning HSV ranges by hand is brutal. You can't tell if your `H_min` should be 8 or 12 without seeing the mask live. So I made a debug script with sliders.

1. Run `raspi_visao_debug.py` on my Mac with the camera plugged in (port 0, `CAP_AVFOUNDATION`).
2. A GUI shows the live frame, the mask, and trackbars for `H_min/H_max`, `S_min/S_max`, `V_min/V_max`, plus area and pixel-percentage thresholds.
3. Slide until the mask is clean — only the target ball shows a green circular outline, everything else is ignored.
4. Hit save. The script writes everything to `raspi_config.json`.
5. Copy the JSON to the Pi via SSH (or VS Code Remote-SSH).

The Pi-side script (`raspi_visao.py`) loads that same JSON at startup and runs headless. This way I never have to edit code to retune — re-run the debug tool, copy the JSON, done on next launch.

---

### `separator_visao.py` — separator chute (built, not used)

This one was built and tested working but didn't make it into the final run. The idea was to watch the separator chute and tell warm balls (ripe) from cool balls (overripe) by color, so the robot could route them to different bins.

The pipeline is simpler than `raspi_visao` — no circle fitting, just ROI + color masking:

1. **Crop two ROIs** from the frame, defined in `separator_config.json` (`roi_x/y/w/h` and `roi_x1/y1/x2/y2`). Everything outside the ROIs is ignored, which kills 90% of the noise from the chute walls and lighting.
2. **HSV conversion** of each ROI.
3. **Two HSV masks per ROI** — one for warm balls (`warm_h_lo` to `warm_h_hi`, plus `warm_s_min` and `warm_v_min`), one for cool balls (`cool_h_lo` to `cool_h_hi`, etc.). Warm sits in the red/orange/yellow band, cool sits in the blue/cyan band.
4. **Black filter** via `black_v_max` to ignore shadows and dark spots that would otherwise sneak through.
5. **Morphological opening** (`morph_k`, `morph_iter`) to clean up speckle noise.
6. **Pixel-fraction decision** — if the percentage of warm or cool pixels in the ROI is over `warm_frac` / `cool_frac` (and the contour area is between `area_min` and `area_max`), the corresponding bit gets set. A `ghost_frames` counter holds the detection for a few extra frames so a single dropped frame doesn't make the bit flicker.

The bits set here are bits 2 and 3 of the vision byte (warmBall, coolBall), so the dispatcher merges them with whatever `raspi_visao` is reporting and the Teensy reads them through the same `Vision` interface.

---

### `benefits.py` — colored box detection (built, not used)

Same fate as the separator — works, just didn't make it into the final routine. The benefits phase needed the robot to recognize a red or blue box and route balls accordingly.

Pipeline:

1. **Single ROI** crop from `benefits_config.json` (`roi_x1/y1/x2/y2`).
2. **HSV conversion**.
3. **Red mask** — red is the painful color in HSV because hue wraps around 0/180, so the config has **two** ranges (`red_h_lo1`/`red_h_hi1` near 0 and `red_h_lo2`/`red_h_hi2` near 180) that get `OR`-ed together. Plus `red_s_min` and `red_v_min` to drop washed-out reds.
4. **Blue mask** — single hue range (`blue_h_lo` to `blue_h_hi`) with its own saturation/value floors.
5. **Morphological opening** to clean speckle (`morph_k`, `morph_iter`).
6. **Pixel-fraction decision** — whichever mask passes `min_pct` first wins. The script then sends `0xFE` (box header) followed by the box type (`1` = red, `2` = blue) instead of a vision bitfield. The Teensy parser handles `0xFE` as a multi-byte message (this is what the persistent parser-state was built for).

Both `separator_visao.py` and `benefits.py` use the same calibration workflow as `raspi_visao` — Mac debug tool, slide HSV ranges live, save to JSON, scp to the Pi.

---
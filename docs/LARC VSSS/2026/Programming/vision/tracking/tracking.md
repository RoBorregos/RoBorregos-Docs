# Tracking

Classification produced a list of robots and (maybe) a ball, each with a position in **pixel
coordinates** and a heading angle, *for one frame in isolation*. Tracking is what turns that into a
**stable, smoothed, predictive** representation in **real-world coordinates**, persisting across frames
even when an object briefly disappears.

In one sentence: classification gives the system *what is in this frame right now*, tracking gives it
*where each object is, where it's been, and where it's going*.

## Two responsibilities, one stage

The tracking stage in this codebase combines two things that are sometimes split into separate stages
elsewhere:

1. **Coordinate transformation**, pixel-space `(x, y)` from classification → meter-space `(x, y)` on
   the field. This is `Coordinates::pixel_to_meter` and is built on a homography computed from the ROI
   corners.
2. **Temporal filtering**, for each tracked object (every robot + the ball), maintain a per-object
   **Kalman filter** that smooths the noisy per-frame measurements into a coherent trajectory and a
   velocity estimate.

The first is straightforward geometry. The second is what most of `tracker.cpp` is about.

## Why tracking is needed

Classification is per-frame and *stateless*. Every frame, it builds a fresh list of robots from
scratch. That has three problems:

- **Noise.** Centroids jitter by 1–2 pixels frame to frame just from classification noise. In real-world
  coordinates that's millimeters, but downstream controllers reading positions at video rate see this
  as high-frequency velocity noise.
- **Disappearance.** A robot can vanish for a frame or two, partial occlusion, a calibration patch
  flickering between red and orange, a marker washed out by glare. Without tracking, the controller
  sees a robot teleport.
- **No velocity.** Classification has no notion of motion. The controllers need velocity for any
  predictive control (intercepting the ball, dodging another robot). It has to come from somewhere,
  and that somewhere is the tracker.

Tracking handles all three by keeping a stateful `Entity` per known object, fed by a Kalman filter.

## Coordinate transformation

Before any temporal logic runs, every detected position is converted from pixels to meters via
`Coordinates::pixel_to_meter`:

```cpp
cv::Point2f pos = coordinates->pixel_to_meter(r_patch.center);
```

The `Coordinates` class holds a 3×3 homography matrix computed once at calibration time
(`update_matrix()`), from the four ROI corners to the four real-world field corners (`-w/2, +h/2` to
`+w/2, -h/2`). The field dimensions are hardcoded:

```cpp
FIELD_WIDTH_M  = 1.50f
FIELD_HEIGHT_M = 1.30f
```

`pixel_to_meter` then does a `cv::perspectiveTransform` of the input pixel through that homography, and
the result lands in field coordinates centered on the field's geometric center.

A useful invariant: after this call, every position is **independent of camera resolution, camera
position, or lens choice**. If the camera moves, the homography changes (after re-running ROI
calibration), but the meter-coordinates of a robot at a given physical spot stay the same.

There's a subtlety in `get_sorted_corners`: the operator can click the ROI corners in *any* order, and
this function infers which is top-left, top-right, bottom-right, and bottom-left from the sums and
differences of `x + y` and `x - y`. So calibration is forgiving about ordering, but the trick assumes
the polygon is roughly axis-aligned with the camera. A heavily rotated camera could fool this
heuristic.

The class also offers `meter_to_minimap` (for the debug minimap), `meter_to_pixel_scalar` (for sizing
debug overlays), `get_warped_image` (for rectifying the camera frame to a top-down view), and
`warped_to_pixel` (the inverse). These are all auxiliary helpers; the tracker really only depends on
`pixel_to_meter`.

## The Entity

Every tracked object, every robot, plus the ball, is an `Entity`:

```cpp
struct Entity {
    int id = -1;
    cv::Point2f pos{0, 0};
    cv::Point2f vel{0, 0};
    double facing = 0.0;
    bool visible = false;

    int blind_frames = 0;
    bool initialized = false;

    long long total_frames = 0;
    long long seen_frames = 0;
    float accuracy = 0.0f;

    cv::KalmanFilter kf;
    cv::Mat measurement;
    // ...
};
```

Robots live in `std::map<int, Entity>`, keyed by robot ID. The ball is a single named `Entity ball`
with `id = 20`.

Each entity carries:

- **Pose**, `pos`, `vel`, `facing`. The output the rest of the system reads.
- **Visibility flags**, `visible` (was this entity *measured* this frame?), `blind_frames` (how many
  consecutive frames since the last measurement?), `initialized` (have we ever seen this entity?).
- **Accuracy stats**, `total_frames` and `seen_frames`, with their ratio displayed as a percentage in
  the debug view. A robot that's been visible 95 frames out of 100 reads as "95%", useful for noticing
  intermittent calibration problems.
- **The Kalman filter** itself, a 4-state, 2-measurement filter (more on this below).

## The Kalman filter

Each entity owns its own `cv::KalmanFilter`. The filter has:

- **State** (size 4): `[x, y, vx, vy]`, position and velocity in meters and meters per frame.
- **Measurement** (size 2): `[x, y]`, measured position from classification, in meters.
- **Transition matrix**:

  ```
  | 1 0 1 0 |     x'  = x + vx
  | 0 1 0 1 |     y'  = y + vy
  | 0 0 1 0 |     vx' = vx
  | 0 0 0 1 |     vy' = vy
  ```

  Constant-velocity model. Each step assumes the entity moves by its current velocity and the velocity
  itself doesn't change. Acceleration isn't modeled, it shows up as process noise instead.

- **Measurement matrix**: a 2×4 identity in the position columns. Measurements observe `(x, y)` only;
  velocity is never measured directly, only inferred from the change in position.

- **Process noise covariance** (`processNoiseCov`): `1e-4 * I`. How much the filter expects the
  constant-velocity model to be wrong, frame to frame. Low values mean "trust the prediction"; high
  values mean "trust the measurement". `1e-4` is on the smoother side, which is appropriate for VSSS
  robots, they don't change velocity instantaneously.

- **Measurement noise covariance** (`measurementNoiseCov`): `1e-2 * I`. How noisy the per-frame
  measurement is. Set higher than the process noise, which means the filter will smooth aggressively
  (a single bad measurement won't yank the position).

The ratio between these two covariances is what really matters. With process noise 100× smaller than
measurement noise, the filter strongly favors its own predictions over raw observations, the result
is a smooth trajectory but a small lag behind sudden movements.

### One frame's update

`Entity::update` runs once per frame per known entity and is called with an *optional* observation:

```cpp
void update(std::optional<cv::Point2f> observed_pos, std::optional<double> observed_facing);
```

The control flow:

1. **Lazy initialization.** If the entity is not yet initialized and we have an observation, call
   `init` to set the Kalman state to the observed position with zero velocity.
2. **Predict.** `kf.predict()` advances the state by one transition step (`x += vx`, `y += vy`) and
   grows the error covariance.
3. **Correct, or not:**
    - If we have a measurement: `kf.correct(measurement)` blends the prediction with the observation
      using the Kalman gain. `blind_frames` resets to 0, `visible` is true, `seen_frames++`.
    - If we don't: `statePre.copyTo(statePost)`, accept the prediction as the post-state without any
      correction. `blind_frames++`, `visible` is false.
4. **Velocity damping.** Multiply `vx` and `vy` by `0.95`:

   ```cpp
   constexpr float VELOCITY_DAMPING = 0.95f;
   kf.statePost.at<float>(2) *= VELOCITY_DAMPING;
   kf.statePost.at<float>(3) *= VELOCITY_DAMPING;
   ```

   This is *not* part of the constant-velocity model. It's a hand-tuned bleed that prevents the
   velocity estimate from drifting indefinitely when the object is blind. Without damping, an entity
   that stops being observed mid-motion would keep "moving" forever in the predicted state.

5. **Read pose out** of `statePost` into the convenience fields (`pos`, `vel`).
6. **Update facing** (separately from the Kalman filter; see below).

### Facing isn't in the Kalman state

Facing is tracked, but it's not one of the four state variables. The reason is wraparound: angles wrap
from `+π` back to `-π`, and a Kalman filter that can't represent that will produce nonsense when a
robot crosses the seam.

Instead, facing is smoothed with a low-pass filter on the **unit vector**:

```cpp
const double x_curr = std::cos(facing);
const double y_curr = std::sin(facing);
const double x_new = std::cos(raw_angle);
const double y_new = std::sin(raw_angle);

const double x_final = (1.0 - FACING_ALPHA) * x_curr + FACING_ALPHA * x_new;
const double y_final = (1.0 - FACING_ALPHA) * y_curr + FACING_ALPHA * y_new;

facing = std::atan2(y_final, x_final);
```

Mapping the angle to the unit circle, blending two unit vectors, then taking `atan2` of the result is
the standard trick for averaging angles correctly across the wrap. `FACING_ALPHA = 0.6` weights the new
measurement at 60%, a reasonably responsive blend that still smooths the per-frame jitter from the
cross-product disambiguation in classification.

## The tracker loop

`Tracker::update` is called once per frame with the output of classification:

```cpp
void update(const std::vector<RobotPatch>& detected_robots,
            const std::optional<BallPatch>& detected_ball);
```

The flow:

### 1. Update the ball

If a ball was classified, convert its center to meters and feed it to `ball.update`. If not, call
`ball.update(nullopt, nullopt)`, the ball entity will run its predict-only step and increment its blind
counter. Note that the ball never gets a facing measurement (`std::nullopt` is always passed for
facing), because the ball doesn't have one.

### 2. Mark every robot as "not seen this frame" by default

```cpp
std::map<int, bool> seen_this_frame;
for (const auto& pair : robots) {
    seen_this_frame[pair.first] = false;
}
```

This will get flipped to `true` for any robot that gets a measurement.

### 3. Process detections

For each `RobotPatch` from classification:

- **Skip unknowns.** If the patch's `id == -1` (the database lookup didn't recognize the parent + child
  triple), drop it. Better to lose a frame than to fabricate identity.
- **Skip non-infield robots.** The system ships with a hardcoded `infield_objects` set in `main.cpp`
  (`{2, 7, 9, 11, 17, 18, 20}`), only IDs in this set get tracked. This is how the project tells the
  tracker which robots are "ours and our opponents" vs. spurious detections of unused IDs.
- **Lazy-create the entity** in the `robots` map if this is the first time we see this ID.
- **Convert** the patch's pixel center to meters.
- **Negate the facing angle** (`double facing = -r_patch.facing;`), this compensates for the image-y
  axis pointing down vs. the field-y axis pointing up. After negation, a positive facing angle is
  consistent with the field's coordinate system.
- **Call `entity.update(pos, facing)`** to feed the Kalman filter.
- **Mark `seen_this_frame[id] = true`.**

### 4. Process unseen entities

Every robot that *wasn't* updated in step 3 gets `entity.update(nullopt, nullopt)`, the predict-only
path. Their `blind_frames` increments and the Kalman filter keeps extrapolating their trajectory.

### 5. Garbage-collect long-blind robots

```cpp
if (entity.blind_frames > MAX_BLIND_FRAMES) {
    it = robots.erase(it);
    continue;
}
```

If a robot has been invisible for more than `MAX_BLIND_FRAMES = 120` frames (~4 seconds at 30 FPS),
it's removed from the tracker. The next time it's detected, it will lazy-create a fresh entity with a
zeroed-out velocity, rather than continuing from a stale (and probably wildly wrong) Kalman state.

This means the tracker handles "robot left the field" gracefully: after a few seconds, the entity is
forgotten. The ball isn't subject to this, `ball.update` is always called every frame, so its blind
counter resets on every observation but it's never erased.

## Ball prediction

The tracker also produces an explicit **future-position prediction** for the ball, used by the
`Prediction` ROS service that controllers query when they need to intercept the ball:

```cpp
const int time_in_future = 1;
if (elapsed.count() >= time_in_future) {
    fixed_future_ball_pos = ball.pos + (ball.vel * 30 * time_in_future);
    last_prediction_time = now;
    has_prediction = true;
}
```

The constant `* 30` here is "30 frames per second × N seconds = N seconds of motion at the current
velocity." So `fixed_future_ball_pos` is the ball's projected position **1 second** from now, assuming
it keeps moving at its current Kalman-estimated velocity.

Because the Kalman filter outputs velocity in *meters per frame*, multiplying by 30 converts to
meters per second; `time_in_future = 1` means one second ahead. The prediction is recomputed once per
second, not every frame, `elapsed.count() >= time_in_future` gates the update. In between, the same
fixed prediction is kept and shown.

This is a deliberately *crude* projection, straight-line motion, no acceleration model, no collision
prediction. The point is to give the controllers a "where will the ball be in roughly a second"
hint they can use to plan an intercept; finer-grained prediction happens controller-side.

The ROS service offers a more flexible per-request version (`ros_handler.cpp` line 75): a controller
can query the predicted position for any specific `dt`, computed the same way (`pos + vel * dt`).

## Outputs and consumers

Once `update` returns, the tracker's state is what the rest of the system reads. Three downstream
consumers, all in `ros_handler.cpp`:

- **`/vision/objects` topic.** Per-frame broadcast of every tracked entity's position, velocity, and
  facing. This is the canonical real-time data feed for the strategy/controller layer.
- **TF transforms.** Each entity's position is published as a `geometry_msgs::TransformStamped` so
  ROS-aware visualizers (RViz) can render the field state directly.
- **`/vision/predict` service.** Controllers can ask "given a robot/ball ID and a time delta, where
  will it be?" The service uses `pos + vel * dt` straight from the tracked entity.

There's also a debug visualization (`Tracker::display_debug_image`) that renders a top-down minimap of
the field with grid lines, goal areas, penalty marks, all the tracked entities (green if visible, yellow
if blind), their facing arrows, accuracy percentages, and the predicted future ball position. This is
shown when `debug_mode` is on, alongside the main camera view.

## Tuning summary

| Parameter | Where | What it controls |
|-----------|-------|------------------|
| `processNoiseCov` (`1e-4 * I`)        | `Entity::init` | How much the filter "trusts" its constant-velocity prediction. Lower = smoother, but laggier. |
| `measurementNoiseCov` (`1e-2 * I`)    | `Entity::init` | How much the filter "trusts" each individual measurement. Higher = more smoothing of per-frame jitter. |
| `errorCovPost` (`1 * I`)              | `Entity::init` | Initial uncertainty. Quickly converges to a value driven by the noise covs. |
| `VELOCITY_DAMPING` (`0.95`)           | `Entity::update` | How fast velocity bleeds toward zero when an entity is blind. |
| `FACING_ALPHA` (`0.6`)                | `Entity::update` | Weight of the new facing measurement vs. the smoothed prior. Higher = more responsive, less smooth. |
| `MAX_BLIND_FRAMES` (`120`)            | `Tracker` member | How many consecutive blind frames before a robot is forgotten. ~4 seconds at 30 FPS. |
| `infield_objects` set                 | `main.cpp` | Which robot IDs to actually track. Anything else is dropped. |
| `time_in_future` (`1`)                | `display_debug_image` | Seconds ahead the ball-prediction overlay extrapolates. |

The knobs you actually touch are mostly the noise covariances. If trajectories look choppy, *lower* the
process noise (or *raise* the measurement noise, they're the same effect, just from opposite sides).
If trajectories lag noticeably behind real motion, do the opposite. Anything else is rarely worth
changing.
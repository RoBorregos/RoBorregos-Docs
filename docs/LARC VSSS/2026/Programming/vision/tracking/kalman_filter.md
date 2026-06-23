# Kalman filter

The Kalman filter is the heart of tracking. Every tracked entity (every robot, plus the ball) owns one,
configured identically. This document explains what the filter does, why it's the right tool here, how
each parameter was chosen, and what the practical behavior looks like.

For the broader tracking pipeline that uses the filter, see [tracking.md](./tracking.md). This
document zooms in on the filter itself.

## What a Kalman filter is, briefly

A Kalman filter is a recursive estimator for the state of a linear dynamic system observed through
noisy measurements. Two ingredients drive it:

1. A **process model** - how the state evolves between observations, on its own.
2. A **measurement model** - how the (noisy) observations relate to the (hidden) state.

Each step has two phases:

- **Predict.** Apply the process model to advance the state forward in time, *without* any new
  observation. The predicted state is what the filter "expects" to see.
- **Correct.** When a real measurement arrives, blend it with the prediction. The blend is weighted by
  how much the filter trusts each: more trust in the measurement -> more correction; more trust in the
  prediction -> more smoothing.

The trust weights are derived from the *covariances* the filter is configured with. That's where
tuning happens.

The output of a Kalman filter is two things at every step: an estimate of the state, and an estimate of
how uncertain that estimate is. In this codebase only the state is read; the uncertainty stays
internal.

## Why a Kalman filter for VSSS tracking

The tracking problem matches the Kalman filter's assumptions almost exactly:

- **Linear dynamics.** A robot or ball moving across the field follows roughly straight-line motion
  between frames. At 30 FPS, the change in velocity within a single frame is small.
- **Gaussian noise.** Classification's per-frame position output is jittery in a way that's reasonably
  approximated as zero-mean Gaussian noise, centroid wobble from KNC's per-pixel boundary
  classification, sub-pixel error from the moments-based centroid computation.
- **Real-time constraint.** The filter is `O(1)` per step regardless of history length. There's no
  stored trajectory, no smoothing window, no batch processing. Each frame's update takes the same
  fixed amount of work.
- **Velocity is a free byproduct.** The filter's state includes velocity, even though only position is
  measured. This is exactly what controllers downstream want: position *and* velocity, with no
  numerical-derivative noise.

Other approaches (particle filters, plain low-pass filters, Kalman smoothers) would either be heavier,
less informative, or non-causal. The basic Kalman filter is the right fit.

## State, measurement, and the model

The filter is **4 states, 2 measurements, no controls** (`kf.init(4, 2, 0)`).

### State vector

```
state = [ x  ]    position x  (meters)
        [ y  ]    position y  (meters)
        [ vx ]    velocity x  (meters per frame)
        [ vy ]    velocity y  (meters per frame)
```

Position is in real-world meters (after `pixel_to_meter`). Velocity is in **meters per frame**, not
meters per second, this is a deliberate choice that simplifies the transition matrix (see below).
Conversion to m/s happens at the consumer (multiply by 30 for ~30 FPS).

### Measurement vector

```
measurement = [ x_observed ]
              [ y_observed ]
```

Just the observed position. No velocity measurement; velocity is purely inferred from how positions
change.

### Transition matrix (the process model)

```cpp
kf.transitionMatrix = (cv::Mat_<float>(4, 4) <<
    1, 0, 1, 0,
    0, 1, 0, 1,
    0, 0, 1, 0,
    0, 0, 0, 1);
```

Reading row by row:

```
x'  = 1·x + 0·y + 1·vx + 0·vy   ->  x'  = x + vx
y'  = 0·x + 1·y + 0·vx + 1·vy   ->  y'  = y + vy
vx' = 0·x + 0·y + 1·vx + 0·vy   ->  vx' = vx
vy' = 0·x + 0·y + 0·vx + 1·vy   ->  vy' = vy
```

This is the **constant-velocity model**: each frame, position advances by velocity, and velocity
itself is unchanged. Notice that the "1" in the position rows next to the velocity columns is just
`dt`, baked into the matrix as `dt = 1`. That's why velocity is in m/frame, if velocity were in m/s,
we'd need to know `dt` (the actual frame duration) and bake the right value in here. By choosing
m/frame as the unit, `dt = 1` always, regardless of the true frame rate.

The model has no acceleration term. A robot accelerating, speeding up, turning, hitting another
robot, violates the model. That violation is absorbed by the **process noise** covariance below.

### Measurement matrix

```cpp
kf.measurementMatrix = cv::Mat::eye(2, 4, CV_32F);
```

Which expands to:

```
| 1 0 0 0 |    measured_x = 1·x + 0·y + 0·vx + 0·vy = x
| 0 1 0 0 |    measured_y = 0·x + 1·y + 0·vx + 0·vy = y
```

The measurement is just the position part of the state. Velocity is not directly measured. This is the
"only positions, never velocities" property that lets the filter quietly estimate velocity for free.

## The covariance configuration

The covariance matrices are where the filter gets tuned. There are three at startup:

```cpp
cv::setIdentity(kf.processNoiseCov,     cv::Scalar::all(1e-4));
cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-2));
cv::setIdentity(kf.errorCovPost,        cv::Scalar::all(1));
```

All three are scaled identity matrices. That's a simplification: it assumes x and y are independent and
have the same noise level (true), and that position and velocity have the same process noise (less
true, but a common simplification, there's no obvious reason to set them differently here).

### Process noise: `1e-4 · I`

This represents how much the filter expects the *constant-velocity model itself* to be wrong, between
two frames. A low value means "the model is accurate; trust the prediction." A high value means "the
model is rough; expect the truth to drift around the prediction."

`1e-4` is small. The filter is being told that, frame to frame, the constant-velocity model is very
accurate, which makes sense, because at 30 FPS one frame is ~33ms and a robot's velocity barely
changes in 33ms.

The consequence is that the filter resists rapid changes in the state. A single sharp measurement
won't move the state much; it takes several consistent measurements in the same direction to convince
the filter that the state is genuinely changing.

### Measurement noise: `1e-2 · I`

This represents how much the filter expects each individual measurement to be wrong. Higher value =
more skeptical of measurements, more smoothing applied.

`1e-2` corresponds roughly to a measurement standard deviation of `√(1e-2) = 0.1` meters = 10 cm. That's
on the high side, actual centroid noise is closer to a few millimeters in field coordinates. Setting
it higher than the true noise level is a deliberate choice: the filter ends up being conservative,
favoring its own predictions over noisy observations, which produces smooth trajectories.

### Initial error covariance: `1 · I`

`errorCovPost` is the *initial* uncertainty in the state estimate, before any observations have been
processed. Setting it to identity (effectively "we don't know anything") works fine, after a handful
of correct steps, the error covariance converges to a value driven by the process and measurement
noise covariances. The initial value is mostly forgotten within the first few frames.

### The ratio is what matters

The ratio between process noise and measurement noise, `1e-4` vs `1e-2`, a factor of 100, is what
determines the filter's behavior. That ratio is the steady-state Kalman gain in disguise:

- **Low process noise / high measurement noise** -> filter trusts its predictions; smooth output, small
  lag behind sudden moves.
- **High process noise / low measurement noise** -> filter trusts measurements; responsive output, more
  jitter.

The 100× ratio in this codebase puts the filter firmly in "smooth and conservative" territory. This is
the right choice for VSSS: robots move predictably, and a slightly laggy but coherent trajectory is
more useful to a controller than a responsive but noisy one.

If both covariances were scaled by the same factor (e.g. `1e-3` and `1e-1`), the filter's behavior would
be **identical**. Only their ratio enters the Kalman gain calculation. So when tuning, change one of
them, not both.

## Initialization

The filter is initialized lazily, on the first observation:

```cpp
if (!initialized && observed_pos.has_value()) {
    init(observed_pos.value(), observed_facing.value_or(0.0));
}
```

`Entity::init` sets:

- `statePost = [start_pos.x, start_pos.y, 0, 0]`, observed position, zero initial velocity.
- The transition, measurement, and noise matrices (as described above).
- `total_frames = seen_frames = 1`, `accuracy = 1.0f`, `initialized = true`.

The zero-velocity start is correct on average: a robot is just as likely to be moving left as right
when first seen. The filter converges on the actual velocity within a few frames as it watches the
position evolve.

If we have no observation and the entity is uninitialized, the filter just doesn't run at all
(`if (!initialized) return;`). There's nothing to predict from.

## One step of the filter

The complete per-frame logic in `Entity::update` for an already-initialized entity:

```cpp
total_frames++;

kf.predict();                          // [1] forward step

if (observed_pos.has_value()) {
    measurement.at<float>(0) = observed_pos->x;
    measurement.at<float>(1) = observed_pos->y;
    kf.correct(measurement);           // [2a] blend in the observation
    blind_frames = 0;
    visible = true;
    seen_frames++;
} else {
    kf.statePre.copyTo(kf.statePost);  // [2b] no observation, just keep the prediction
    blind_frames++;
    visible = false;
}

kf.statePost.at<float>(2) *= 0.95;     // [3] velocity damping
kf.statePost.at<float>(3) *= 0.95;
```

### [1] Predict

`kf.predict()` does:

```
statePre        = transitionMatrix · statePost   // advance state by one step
errorCovPre     = transitionMatrix · errorCovPost · transitionMatrix^T + processNoiseCov
```

`statePre` is now the predicted state at this frame *before* any observation is considered. The error
covariance grows with each prediction step (we're more uncertain after extrapolating). `processNoiseCov`
is what controls how much it grows.

### [2a] Correct (with measurement)

`kf.correct(measurement)` does:

```
y = measurement - measurementMatrix · statePre        // innovation
S = measurementMatrix · errorCovPre · measurementMatrix^T + measurementNoiseCov
K = errorCovPre · measurementMatrix^T · S^(-1)        // Kalman gain
statePost = statePre + K · y
errorCovPost = (I - K · measurementMatrix) · errorCovPre
```

The Kalman gain `K` is the key quantity. It controls how much of the innovation (the difference
between what we measured and what we predicted) gets blended in. With low process noise and high
measurement noise, `K` is small, and the post-state is dominated by the prediction. With high process
noise and low measurement noise, `K` is large, and the post-state is dominated by the measurement.

The error covariance always shrinks after a correction step, the observation has added information.

### [2b] No measurement: just accept the prediction

```cpp
kf.statePre.copyTo(kf.statePost);
```

When there's no observation, this line accepts the prediction as the state without any correction. The
error covariance is *not* updated, meaning the next predict step will inflate it further. This is
correct: the longer we go without observations, the more uncertain we should be.

This is the path that handles **occluded or briefly mis-classified entities**. The filter keeps
extrapolating the trajectory based on the last known velocity, and the entity's `pos` field continues
to make sense for downstream consumers.

### [3] Velocity damping (not part of the Kalman filter!)

The last two lines:

```cpp
kf.statePost.at<float>(2) *= 0.95;
kf.statePost.at<float>(3) *= 0.95;
```

These are **not** part of the Kalman filter. They're an ad-hoc post-processing step that bleeds
velocity toward zero by 5% per frame. Why?

The constant-velocity model assumes velocity is constant. But the observed velocity comes from
position differences, which means it's noisy. When the entity is not being observed (`statePre` is
just copied to `statePost`), the noisy velocity from the last observation keeps propagating
unchecked: position keeps drifting at whatever velocity we last estimated.

Without damping, an entity that briefly disappears at full speed would keep "moving" at that speed
indefinitely. That's clearly wrong for a real-world robot, which usually decelerates when it can't be
tracked (because something occluded it, often because it stopped or got tangled).

The 5%-per-frame damping is a hand-tuned bleed: at 30 FPS, velocity decays by ~78% per second
(`0.95^30 ≈ 0.215`), so within a couple of seconds of being blind, an entity is effectively stationary
at its last known position. This matches the physical intuition: "if I stop seeing a robot, it
probably stopped moving."

In a "pure" Kalman framework this would be expressed as a process model with a velocity-decay term
(equivalent to a known acceleration toward zero, scaled by velocity). The `0.95 * vx` shortcut is
mathematically equivalent for the state update, just implemented outside the filter machinery.

## Reading state out

After steps [1]–[3], the convenience fields are populated:

```cpp
pos.x = kf.statePost.at<float>(0);
pos.y = kf.statePost.at<float>(1);
vel.x = kf.statePost.at<float>(2);
vel.y = kf.statePost.at<float>(3);
```

These are the values the rest of the system reads via `Tracker::ball.pos`,
`Tracker::robots[id].vel`, etc. The internal Kalman state objects (`statePre`, `statePost`, the
covariances) are never exposed.

`vel` is in m/frame; multiplying by 30 (or whatever the actual frame rate is) gives m/s. The
ROS publisher uses `vel.x` and `vel.y` directly without the conversion, which is technically
inconsistent, be aware of that if you read from the topic.

## What the filter does *not* track

Three things are deliberately outside the Kalman filter:

- **Facing.** Angles wrap, the filter assumes linear dynamics. Facing is smoothed separately via a
  unit-vector low-pass (covered in [tracking.md](./tracking.md)).
- **Acceleration.** The constant-velocity model has no acceleration. Acceleration shows up as
  process noise, which the filter then smooths through.
- **Identity.** The filter is per-entity. There's one filter per robot, keyed by ID. If a robot's ID
  changes mid-track (e.g. classification flips between two similar IDs frame to frame), the result is
  two separate filters, each underfed. The fix is in classification, not here.

## Practical behavior

Some intuitions about how the filter responds:

- **Cold start.** First observation initializes pos to the observation, vel to zero. Velocity converges
  on the true value within ~5–10 frames as the filter watches positions evolve.
- **Steady straight-line motion.** Filter output is essentially identical to the raw measurement,
  smoothed by a fraction of a centimeter. Velocity is stable and accurate.
- **Sudden turn.** The filter lags behind the measurement for several frames as the prediction's
  inertia (constant velocity in the *old* direction) gets corrected by new measurements. Trajectories
  through sharp turns are visibly smoothed.
- **One-frame dropout.** Single missed measurement. The prediction step extrapolates one frame at the
  current velocity; visually invisible.
- **5–10 frame dropout.** Position drifts along the last velocity vector for ~10 frames (~0.3s at 30
  FPS). Velocity damping has shaved off ~40% of the velocity by the time the entity reappears.
  Trajectory snaps back to the new measurement on the next correction.
- **Long dropout (> `MAX_BLIND_FRAMES = 120`).** Tracker garbage-collects the entity. Next sighting
  starts a fresh filter with zero initial velocity.

## Tuning summary

| Parameter | Default | Effect of increasing it |
|-----------|---------|-------------------------|
| `processNoiseCov` | `1e-4` | More responsive, less smooth |
| `measurementNoiseCov` | `1e-2` | More smooth, less responsive |
| `errorCovPost` (initial) | `1` | Slightly slower convergence in the first few frames; nearly invisible after that |
| `VELOCITY_DAMPING` | `0.95` | Velocity decays *faster* when blind. (Lower the value, faster the decay.) |

The two covariances are the only knobs worth touching, and only their *ratio* matters. Start there if
the trajectories look wrong.
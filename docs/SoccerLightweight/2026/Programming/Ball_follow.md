# Ball follow

This note documents the current Colibri/PIDLookForBall IR chase logic and the tuning knobs that matter most on the field.

## Heading settle band

kHeadingSettleBandDeg is passed into the heading PD controller as the angle band where the robot is considered close enough to the target yaw. In PIDLookForBall.cpp it is currently 5.5f; in Colibri.cpp it is tuned separately at 6.0f.

This value is small, but powerful. A tighter band makes the robot fight harder to keep the startup heading while it drives toward the ball. That can improve straight-line attacks, but it can also add twitching because the robot is already changing drive direction quickly from the IR angle. A wider band makes the robot calmer and lets the chase vector dominate, but too much width might allow visible yaw drift.

Tune this with kHeadingKp, kHeadingKd, kMaxTurnPwm, and kMinTurnPwm, not in isolation. The best value is the smallest band that does not cause steering chatter during fast ball-angle changes.

## Angle wrapping

`DriveHelpers::wrapAngle180(angleDegrees)` normalizes any angle into [-180, 180].

That matters because the rear of the robot is the discontinuity. For example, 181 degrees and -179 degrees are almost the same physical direction, but without wrapping they look 360 degrees apart to control logic. Wrapping keeps the robot from making a huge correction when the ball crosses the rear angle boundary.

## UART IR packet handling

PIDLookForBall.cpp supports three packet forms from the IR processor:

- a <angle>: accepted ball angle packet.
- r <...>: rejected packet, ignored by the chase loop.
- <angle>: bare numeric angle for older/simple IR sketches.

Accepted angles are converted with:

```cpp
DriveHelpers::wrapAngle180(180.0f - incomingAngle)
```

The 180.0f - incomingAngle conversion maps the IR sensor coordinate frame into the drivetrain coordinate frame. The wrap then prevents rear-boundary jumps from becoming false large turns.

Colibri.cpp assumes the Teensy-facing IR processor is already sending a clean signed ball angle per line. Keeping the IR ring calculation off the main Colibri loop is a major speed advantage: Colibri can spend its loop time on serial intake, heading hold, line avoidance, and motor output instead of recomputing sensor geometry. That lowers latency from sensor update to motor command, which is exactly what ball following needs.

## Behind-ball orbit

getBehindBallOrbitAngle(float ballAngle) is the key helper that makes the chase smarter when the ball is behind the robot:

```cpp
const float orbitDirection = (ballAngle >= 0.0f) ? 1.0f : -1.0f;
const float rearHalfErrorDeg = fabsf(ballAngle) - 90.0f;
const float orbitAdjustment = DriveHelpers::clampSymmetric(
    rearHalfErrorDeg * kBehindBallOrbitAdjustGain,
    kBehindBallOrbitAdjustMaxDeg
);
return DriveHelpers::wrapAngle180(ballAngle - orbitDirection * orbitAdjustment);

```

When fabsf(ballAngle) > 90, the ball is in the rear half. Driving directly at that angle can make the robot back into the ball or push from the wrong side. This helper bends the drive angle sideways so Colibri orbits around the ball and approaches from a better attack position.

kBehindBallOrbitAdjustGain controls how quickly the orbit correction grows as the ball moves deeper behind the robot. kBehindBallOrbitAdjustMaxDeg caps the correction so the robot still moves toward the ball instead of circling forever.

These constants are high-leverage tuning. A good pair makes the robot look dramatically faster because it avoids bad backward contacts and spends less time recovering. Too little adjustment looks sluggish and direct. Too much adjustment makes the robot over-orbit and miss the shortest path back to the ball.

## Colibri state machine

Colibri currently has two states:

- CHASE_BALL: read the latest IR angle, optionally bend the chase angle for behind-ball orbiting, map it to the drivetrain frame, and drive while holding startup yaw.
- AVOIDING_LINE: when the photo library reports a field line, remember the escape angle and drive away for Constants::kAvoidDurationMs, then return to CHASE_BALL.

The state machine keeps line avoidance from fighting ball chase. Once a line is detected, line escape owns the motors for a short, predictable time. After that, the robot returns to the IR ball chase path.

## Feature booleans and helpers

The main behavior switches are:

- kEnableDebugPrints: enables compact serial status output.
- kEnablePixy: controls whether Pixy initialization runs.
- kEnableLineAvoidance: enables the photo-library line detection and escape state.

The important helpers are:

- mapRobotBallAngleToDriveAngle: converts the robot-relative ball angle into the current drivetrain frame.
- getBehindBallOrbitAngle: bends rear-half ball angles into an orbiting approach.
- acceptBallAngle: commits a validated IR angle and updates freshness timing.
- retakeGreenBaseline: refreshes photo baselines after line events.
- DriveHelpers::wrapAngle180: shared angle normalization.
- DriveHelpers::clampSymmetric: limits steering/orbit corrections without changing sign.

Line detection, per-side photo readings, side baselines, and field-line escape angles are owned by lib/Photos. Colibri should call those library functions instead of duplicating sensor-side logic locally.
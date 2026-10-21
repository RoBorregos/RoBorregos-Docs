# Ball Follow

Ball follow is the striker behavior that converts the IR ball angle into robot movement. The IR ring tells us where the ball is relative to the robot, and the striker uses that angle to decide how to move while still keeping its body orientation stable with the BNO and heading PD controller.

This behavior is important because driving directly toward the ball is not always the best play. If the ball is in front of the robot, we can chase it directly. If the ball is behind the robot, driving straight backward can push the ball in the wrong direction or make the robot lose time recovering. For that reason, we added orbiting logic so the striker can move around the ball and approach it from a better angle.

## Main Concepts

The ball-follow code works with three important angles:

- `rawTheta`: the latest ball angle received from the IR processor.
- `ballAngle`: the cleaned angle we use for chase decisions.
- `driveAngle`: the final movement angle sent to `robot.motors.move()`.

The robot also tracks its yaw using the BNO. Yaw is the direction the robot body is facing. While the drivetrain moves toward the ball, the heading PD controller keeps the robot from rotating away from its desired orientation.

## Reading the Ball Angle

The striker receives the IR angle through `Serial3`. The IR processor sends a signed ball angle, so the main striker loop does not need to recalculate all IR sensor geometry. This keeps `Striker.cpp` focused on fast decisions: reading serial data, checking line sensors, holding heading, and updating motors.

The latest accepted ball angle is stored with `acceptBallAngle()`. This updates:

- `currentBallAngle`
- `latestRawBallTheta`
- `latestBallAngle`
- `lastBallReadMs`

We also added angle-jump filtering. If a new angle changes too suddenly, using `kBallAngleJumpIgnoreDeg`, the code does not accept it immediately. If a similar jump repeats within `kBallAngleRepeatToleranceDeg`, then we accept it. This helps ignore one-frame IR noise without blocking a real fast ball movement.

## Angle Wrapping

`DriveHelpers::wrapAngle180(angleDegrees)` keeps angles inside the `-180` to `180` degree range.

This matters because the rear of the robot is a discontinuity. For example, `181` degrees and `-179` degrees are almost the same physical direction, but without wrapping they look like a `360` degree jump. Wrapping prevents the robot from making a huge correction when the ball crosses behind it.

## Mapping to the Drivetrain

The IR angle and the drivetrain angle are not the same frame of reference. The UART ball angle uses `0` as the front of the robot and `+/-180` as the back. The drivetrain response is mirrored front-to-back, so we map the ball angle with:

```cpp
DriveHelpers::wrapAngle180(180.0f - robotAngle)
```

This conversion happens in `mapRobotBallAngleToDriveAngle()`.

## Behind-Ball Orbit

The biggest improvement in ball follow was adding orbit behavior when the ball is behind the robot.

Before this, the striker could drive directly at the rear ball angle. That often made the robot back into the ball or contact it from a bad side. Instead of improving the attack, the robot could push the ball away from the goal or spend time recovering.

Now, when `fabsf(chaseAngle) > 90.0f`, the code treats the ball as being in the rear half and calls `getBehindBallOrbitAngle()`.

```cpp
const float orbitDirection = (ballAngle >= 0.0f) ? 1.0f : -1.0f;
const float rearHalfErrorDeg = fabsf(ballAngle) - 90.0f;
const float orbitAdjustment = DriveHelpers::clampSymmetric(
    rearHalfErrorDeg * kBehindBallOrbitAdjustGain,
    kBehindBallOrbitAdjustMaxDeg
);
return DriveHelpers::wrapAngle180(ballAngle - orbitDirection * orbitAdjustment);
```

`orbitDirection` tells the robot which side the ball is on. If `ballAngle` is positive, the ball is on one side of the robot, so the correction is applied in that direction. If `ballAngle` is negative, the correction is mirrored to the other side.

`rearHalfErrorDeg` measures how far into the rear half the ball is. A ball at `90` degrees is exactly on the side of the robot, so the rear error is `0`. A ball at `150` degrees is much deeper behind the robot, so the rear error is larger.

`orbitAdjustment` multiplies that rear-half error by `kBehindBallOrbitAdjustGain`. This means the deeper the ball is behind the robot, the more the striker bends its movement sideways instead of driving straight backward.

`DriveHelpers::clampSymmetric()` limits that correction using `kBehindBallOrbitAdjustMaxDeg`. This is important because without a maximum limit, the robot could bend the movement angle too much and stop making useful progress toward the ball.

Finally, the function subtracts the correction from `ballAngle` and wraps the result back into the `-180` to `180` degree range. The result is a new chase angle that makes the striker arc around the ball instead of backing directly into it.

## Orbit Tuning

The orbit behavior depends mainly on two constants:

- `kBehindBallOrbitAdjustGain`: controls how quickly the robot starts arcing around the ball.
- `kBehindBallOrbitAdjustMaxDeg`: limits the maximum orbit correction.

If `kBehindBallOrbitAdjustGain` is too low, the robot still behaves too directly and may hit the ball from behind. If it is too high, the striker can circle too much, overshoot the approach, and never actually touch the ball because it keeps orbiting around it instead of closing distance.

If `kBehindBallOrbitAdjustMaxDeg` is too low, the orbit correction is barely noticeable. If it is too high, the striker can take a longer path than needed and miss the shortest route back to the ball.

## Chase Speed

We use different speeds depending on the situation:

- `kChaseDrivePwm`: normal ball chase speed.
- `kBehindBallDrivePwm`: speed used when orbiting behind the ball.
- `kAvoidDrivePwm`: speed used while escaping a line.

The behind-ball speed can be higher because the robot is intentionally trying to reposition quickly around the ball. This should still be tuned carefully so the phototransistors have enough time to detect lines.

## Heading Control

The heading PD controller keeps the robot facing the startup yaw while it moves. This lets the striker translate toward the ball without freely spinning.

The most important heading constants are:

- `kHeadingKp`
- `kHeadingKd`
- `kMaxTurnPwm`
- `kMinTurnPwm`
- `kHeadingSettleBandDeg`

`kHeadingSettleBandDeg` defines how close the robot must be to the target yaw before the controller stops correcting. A smaller value holds direction more aggressively, but it can make the robot twitch. A larger value makes the robot calmer, but it can allow more yaw drift.

## Line Avoidance Priority

Ball follow does not control the robot all the time. Line avoidance has priority.

If `phototransistor_sensors.CheckPhotosOnField()` detects a line, the striker enters `AVOIDING_LINE`. In this state, the robot stores the escape angle, drives away using `kAvoidDrivePwm`, and ignores ball chase until `Constants::kAvoidDurationMs` has passed.

This prevents ball chase and line escape from fighting over the motors.

## Current State Machine

The striker currently uses two main states:

- `CHASE_BALL`: read the latest IR angle, optionally orbit if the ball is behind, map the angle to the drivetrain frame, and move while holding yaw.
- `AVOIDING_LINE`: temporarily drive away from a detected line, then return to `CHASE_BALL`.

The normal loop is:

1. Read the newest ball angle from `Serial3`.
2. Read current yaw from the BNO.
3. Calculate the heading correction with the PD controller.
4. Check whether line avoidance should take over.
5. If no line is detected, decide whether the ball is behind the robot.
6. If the ball is behind, bend the chase angle with `getBehindBallOrbitAngle()`.
7. Convert the robot-relative ball angle into a drivetrain angle.
8. Send `driveAngle`, drive PWM, and turn correction to `robot.motors.move()`.

## Debugging

When `kEnableDebugPrints` is enabled, the code prints useful values such as:

- current state
- yaw
- raw ball angle
- chase angle
- drive angle
- turn command
- active line side
- whether the ball is in front
- whether we currently have a ball reading

These debug values are useful when tuning orbiting because they show whether the issue is coming from the IR angle, the orbit correction, the drivetrain mapping, or the heading controller.

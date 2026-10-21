## Current software summary

### Current striker software behavior

- On startup it captures phototransistor baseline values and locks onto the opponent goal using Pixy `SIG2` and `SIG3`.

- During play it checks line first, then keeps PID heading stable with the IMU, then uses the UART IR ball angle to chase the ball.

- When the ball is centered enough and the opponent goal is visible, it uses Pixy goal position to aim before pushing forward to shoot.

- If the ball angle data is lost, it stops translation and only keeps heading correction.

### Current goalie software behavior

- On startup it captures phototransistor baseline values and locks onto its home goal using Pixy `SIG2` and `SIG3`.

- During play it checks line first, then keeps PID heading stable with the IMU, then uses Pixy to stay centered with its own goal.

- If the ball angle becomes threatening and the goalie is still inside its safe home-goal zone, it slides to intercept.

- If the goalie drifts too far from its goal or loses goal tracking, it retreats and re-centers before defending again.

## Startup behavior

### Startup sequence

- The phototransistor system is initialized first.

- Margins for line detection are loaded from `constants.h`.

- A baseline capture is made using several samples so the robot can compare current readings against its starting field condition.

- Motors are held stopped for a short delay so sensor startup is more stable.

- The Pixy then tries to lock one of the two goal signatures.

- The IMU heading is read and stored as the target yaw used by heading PID during movement.

### Why this matters

- If baseline capture is bad, line detection will be noisy or late.

- If goal lock fails at startup, Pixy-based positioning and aiming become weaker until the robot sees the goal again.

- If IMU startup is unstable, the robot will drift or fight itself while moving.

## Main thresholds to tune

### Striker

- `Constants::Striker::kIRPossessionAngleToleranceDeg`: how centered the ball must be before the striker starts aiming the goal instead of only chasing.

- `Constants::Striker::kGoalCenterTolerancePx`: how centered the goal must be in the Pixy frame before the striker considers it lined up.

- `Constants::Striker::kGoalAimAreaThreshold`: how large the goal must look before the striker commits more directly to shooting.

- `Constants::Striker::kIRBallAngleClampDeg`: limits how sharply the striker can cut while chasing.

- `Constants::Striker::kGoalAngleClampDeg`: limits how aggressively Pixy can pull the striker left or right during aiming.

- `Constants::Striker::kChaseDrivePwmRatio`, `kGoalDrivePwmRatio`, `kAvoidDrivePwmRatio`: overall behavior balance between control, aiming, and line escape.

### Goalie

- `Constants::Goalie::kGoalAngleClampDeg`: maximum Pixy correction while centering on the home goal.

- `Constants::Goalie::kGoalCenterDeadbandDeg`: how much goal error is tolerated before the goalie starts correcting.

- `Constants::Goalie::kGoalCorrectionWeight`: how strongly goal-centering affects intercept movement.

- `Constants::Goalie::kBallFollowWeight`: how strongly ball angle affects intercept movement.

- `Constants::Goalie::kIRThreatAngleToleranceDeg`: how wide the threat window is before the goalie decides to react.

- `Constants::kLeftGoalKeeperTresholdX`, `kRightGoalKeeperTresholdX`, `kMinGoalKeeperTresholdY`, `kMaxGoalKeeperTresholdY`: the safe zone used to decide whether the goalie is still protecting the goal correctly.

### Phototransistors and line behavior

- `Constants::kPhotoMargins`: main line detection tuning values for each channel.

- `Constants::kAvoidDurationMs`: how long the robot keeps escaping once line is detected.

- `Constants::kBaselineSamples` and `Constants::kBaselineDelayMs`: baseline stability during startup calibration.

## What we want to improve next

### Main improvement goals

- fewer line mistakes

- faster recovery after losing the ball

- more stable heading while moving

- better decision on when to shoot versus when to keep carrying the ball

### Ideas to improve this

- Improve striker shot selection so it only commits hard to goal when the ball is centered and the goal is stable in Pixy.

- Add more recovery behavior after losing ball angle so the robot does not freeze too long or drift into bad positions.

- Make heading control more consistent while translating fast, especially when switching between chase and goal-aim modes.

- Reduce false line triggers and missed line detections by retuning channel margins and testing on different field lighting.

- Retune goalie safe-zone thresholds so it defends aggressively without drifting too far from goal.

- Add more repeated field tests for corner balls, side entries, rebounds, and traffic from other robots.

## Suggested tuning workflow

- A good next step is to use a concrete field-testing order: change 5 important constants one by one and watch exactly what behavior improves or gets worse.

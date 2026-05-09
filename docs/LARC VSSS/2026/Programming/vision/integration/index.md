# Integration (ROS 2)

This is the final stage of the vision pipeline. Tracking produced a set of stable, smoothed entities
in real-world coordinates. **Integration** is what makes those entities visible to the rest of the
robot system.

The vision binary is one ROS 2 node, `vision_publisher_node`, that:

- **Publishes** robot and ball positions, velocities, and orientations on a topic.
- **Broadcasts** a TF transform tree so other nodes (and visualizers like RViz) can place these
  entities in a common frame.
- **Publishes markers** for visualization, including a fully-drawn field outline.
- **Provides** a prediction service that controllers can call when they need a future position.
- **Subscribes** to a simulator image topic for non-camera execution.

All of this lives in one class: `RosHandler`, which inherits from `rclcpp::Node`. It's also the same
class the GUI/Detector uses to read launch parameters (`debug_mode`, `use_camera`, etc.), so it's the
single integration point between the vision subsystem and ROS 2.

## Where the node sits in the architecture

The node is **not** the main loop. The main loop is the regular OpenCV/CUDA frame loop in
`main.cpp`; ROS 2 is one of several things that happens during each frame:

```cpp
while (rclcpp::ok()) {
    // ... grab frame, run pipeline ...
    gui.upload_frame(image);
    auto detections = detector.update();
    tracker.update(detections.first, detections.second);
    // ...
    gui.display_frame();
    ros_handler->publish_objects();   // publish to ROS
    executor.spin_some();             // service callbacks fire
    cv::waitKey(delay);
}
```

The pattern is: **vision drives the clock, ROS gets serviced cooperatively**. `executor.spin_some()`
runs once per frame and processes any pending subscription / service callbacks (e.g., handling a
`/vision/prediction` request, or receiving a new simulator frame). It returns immediately if there's
no work to do, so it doesn't block the frame loop.

This is a deliberate choice. A more conventional ROS 2 design would put the frame loop inside a
timer callback and let ROS drive everything; here, the vision pipeline's frame rate is the
authoritative clock and ROS is plugged in as a publisher/subscriber on the side. The result is
predictable timing, each frame takes roughly the same amount of work, at the cost of being slightly
unconventional.

The handler is a `shared_ptr` because that's what `rclcpp::executors::Executor::add_node` requires.
It's owned by `main` and kept alive for the duration of the program.

## What gets published, and where

There are four publication channels, all in real-world meter coordinates:

```
/vision/detection    [vsss_vision/EntityState]   , per-entity state stream
/vision/markers      [visualization_msgs/MarkerArray]  , RViz visuals (entities + field)
/tf                  [tf2_msgs/TFMessage]        , transform tree (broadcast via tf2)
/vision/prediction   [vsss_vision/Prediction]    , service for future-position queries
```

Each is described in its own section below.

### Publication rate, the 15 Hz throttle

All publication is gated by a software throttle:

```cpp
float publish_hz = 15;

void RosHandler::publish_objects() {
    const rclcpp::Time now = this->get_clock()->now();
    if (millis() - previous_millis >= (1000 / publish_hz)) {
        publish_tfs(now);
        publish_markers(now);
        publish_field_markers(now);
        publish_legacy_messages();
    }
}
```

Even though `publish_objects()` is called every frame (potentially 30+ Hz), the actual publication
only happens at ~15 Hz. The reasoning: the strategy/control layer downstream doesn't need 60 updates
per second; 15 is enough for control loops, and halving the rate means halving the network load on
the ROS DDS layer.

The throttle has an obvious but subtle bug: `previous_millis` is **never updated** inside the
`if` block. So once the first 1/15s window has elapsed, every subsequent call to `publish_objects`
will publish, until you'd expect, except that's how it works on every frame too. In practice this
means publication happens *every frame* after the first 67ms. If 15 Hz throttling is actually wanted,
`previous_millis = millis()` needs to be added inside the gate.

This is worth flagging because the file looks like it intends to throttle, and downstream consumers
might be assuming the lower rate.

## `/vision/detection`, `EntityState` topic

This is the canonical real-time data feed for the strategy/controller layer. The message definition
is:

```
int32 id
geometry_msgs/Pose position
geometry_msgs/Twist velocity
```

Three fields:

- `id`, the robot's ID (0–19) or `20` for the ball.
- `position`, pose in the `field` frame: `(x, y, 0)` plus a quaternion encoding the heading.
- `velocity`, twist with linear `(vx, vy, 0)` (no angular velocity is computed by the tracker).

`publish_legacy_messages()` publishes the ball first, then iterates over every robot in the tracker.
Each entity becomes a separate message:

```cpp
publisher->publish(ball_message);

for (const auto& robot : tracker->robots) {
    auto robot_message = vsss_vision::msg::EntityState();
    robot_message.id = robot.first;
    // ... fill in pose and velocity ...
    publisher->publish(robot_message);
    std::this_thread::sleep_for(std::chrono::microseconds(1500));
}
```

The 1.5 ms sleep between messages is a paced-publication trick. Publishing N messages back-to-back
through DDS can cause frame coalescence at the receiver, where multiple messages get processed in the
same callback invocation and effectively "blur together" in time. The pacing forces them to land at
slightly different timestamps. Whether this actually helps depends on the QoS of the consumer; it's
defensive code.

The same pacing is used in `publish_tfs` for the same reason.

### QoS

The publisher uses `SensorDataQoS` with `keep_last(10)`:

```cpp
auto qos_profile = rclcpp::SensorDataQoS();
qos_profile.keep_last(10);
publisher = this->create_publisher<vsss_vision::msg::EntityState>("/vision/detection", qos_profile);
```

`SensorDataQoS` is a preset that picks `BEST_EFFORT` reliability, `VOLATILE` durability, and a
short history depth, appropriate for a high-frequency stream of measurements where missing one
frame is fine but blocking on retransmission is not. `keep_last(10)` overrides the depth to 10,
which gives a small buffer for slightly-bursty consumers.

Subscribers should match this QoS or they won't receive messages.

### Quaternion encoding

The message uses a quaternion for orientation, but the tracker only has a 2D heading angle. The
conversion is the standard rotation-around-the-z-axis quaternion:

```cpp
robot_message.position.orientation.z = sin(robot.second.facing / 2.0);
robot_message.position.orientation.w = cos(robot.second.facing / 2.0);
```

(With `x = y = 0` since the rotation axis is purely vertical.) This is the cleanest representation
because it's the standard ROS / TF convention and consumers can use any `tf2` helper to extract Euler
angles or compose with other transforms.

The same conversion is used everywhere a heading is published (TF, markers, EntityState).

### Velocity unit caveat

The tracker stores velocity in **meters per frame** (see [kalman_filter.md](./kalman_filter.md)).
The published `velocity.linear.x/y` fields take this value *directly* without a frame-rate
conversion:

```cpp
robot_message.velocity.linear.x = robot.second.vel.x;
robot_message.velocity.linear.y = robot.second.vel.y;
```

ROS conventions are SI units, m/s, not m/frame. The downstream consumer is implicitly assuming a
fixed frame rate to interpret these values, or treating them as relative magnitudes. If the
camera's frame rate changes (e.g. camera misconfigured at 15 FPS instead of 30), the published
velocities will be off by a factor without anything else changing.

This is a known inconsistency. The fix would be `velocity.linear.x = robot.vel.x * fps`. Worth being
aware of when integrating downstream.

## `/tf`, transform tree

ROS 2 has a global, time-stamped tree of coordinate frames managed by tf2. The vision node publishes
into it via a `TransformBroadcaster`:

```cpp
tf_broadcaster = std::make_unique<tf2_ros::TransformBroadcaster>(this);
```

The tree it builds has a single root frame called `field`, with one child per tracked entity:

```
field
├── ball
├── robot_0
├── robot_1
├── ...
└── robot_19
```

`publish_tfs` is called on each publish cycle and emits one `TransformStamped` per entity. The frame
ID convention is:

- `header.frame_id = "field"`, parent frame.
- `child_frame_id = "robot_<id>"` or `"ball"`, child.

The same rotation-as-quaternion encoding is used here. Translation is the entity's `(x, y, 0)`
position.

The reason this matters: any other node doing geometric reasoning (path planning, formation control,
collision avoidance) can ask tf2 "give me robot_5's pose relative to robot_3" and tf2 will compose
the transforms automatically. The vision node doesn't have to know who its consumers are, it just
publishes positions, and tf2 makes them queryable.

Same pacing (1.5 ms sleep) is used here as in legacy messages.

## `/vision/markers`, RViz visualization

Two separate `MarkerArray` publications go to this topic each cycle:

1. **Entity markers** (`publish_markers`), one sphere for the ball, one cube per robot.
2. **Field markers** (`publish_field_markers`), the static field layout (lines, center circle).

### Entity markers

Each robot is rendered as a 75×75×70 mm cube (real VSSS robot dimensions); the ball is a 42.7 mm
sphere. They're given namespaces (`"robots"`, `"ball"`) and IDs matching the tracker.

The colour encodes both team and visibility:

```cpp
if (robot.team() == ObjectType::Yellow) {
    m.color.r = robot.visible ? 0.8 : 0.6;
    m.color.g = 0.8;
} else {
    m.color.b = 0.8;
    m.color.g = robot.visible ? 0.2 : 0.4;
}
```

A visible robot is bright; an entity that's currently being predicted (Kalman extrapolation, no
measurement this frame) is dimmed. The shift is subtle but useful in RViz for at-a-glance "is this
robot actually being seen right now?" inspection.

The ball uses the same trick, the green channel goes from 0 to 0.5 based on `ball.visible`, so a
recently-observed ball is orange while a Kalman-extrapolated one is solid red.

### Field markers

`publish_field_markers` draws the entire VSSS field as line markers, the outer rectangle, the
midline, the goal areas, the penalty arcs, the cross marks, and the center circle (as a 64-segment
`LINE_STRIP`). All dimensions come from the `FieldSizes` constants header.

This is a static drawing, the field doesn't move, but it gets re-published every cycle anyway,
because RViz markers without a `lifetime` need periodic refreshes to stay alive in the visualizer.
This is wasteful (the field could be published once with `lifetime = 0`, which means "persist
forever"), but it's a minor optimization.

The end result in RViz is a textbook representation of the playing field with the live entities
moving on top of it, useful for verifying that the homography is correct (entities should always
land *inside* the drawn field, not on top of the boundary lines or in negative space).

## `/vision/prediction`, service

This is the one **request/response** interface the vision node exposes. Controllers needing to plan
ahead, "where will the ball be in 0.4 seconds?", call this service rather than the topic.

The service definition:

```
# Request
int32 id
float64 seconds_in_future
---
# Response
std_msgs/Header header
int32 id
geometry_msgs/Pose predicted_position
```

The implementation is a straight-line projection from current state and Kalman velocity:

```cpp
double dt = request->seconds_in_future;
int target_id = request->id;

if (target_id == 20) {
    response->predicted_position.position.x = tracker->ball.pos.x + (tracker->ball.vel.x * dt);
    response->predicted_position.position.y = tracker->ball.pos.y + (tracker->ball.vel.y * dt);
}
else if (tracker->robots.count(target_id)) {
    auto& robot = tracker->robots[target_id];
    response->predicted_position.position.x = robot.pos.x + (robot.vel.x * dt);
    response->predicted_position.position.y = robot.pos.y + (robot.vel.y * dt);
    response->predicted_position.orientation.w = 1.0;
}
```

Note the unit issue surfaces here too: `vel.x * dt` is `(m/frame) × (seconds)`. The result has units
of `m·s/frame`, not meters. For the prediction to actually be in meters, the velocity needs to be in
m/s, same caveat as the EntityState publication.

The service is intentionally minimal. There's no acceleration, no collision, no model of the
environment, it's just a linear extrapolation. Controllers that need something fancier (e.g. ball
bouncing off walls) compute it on their side.

If `target_id` is neither 20 nor a known robot, the response is left zero-initialized and returned.
There's no error code or status field, the caller has to detect "no data" by checking if the
position is `(0, 0)` (which is the field center, an entirely valid position too, be careful).

## `/vision/sim_image`, subscription

This is the *only* subscription the vision node has, and it's used in **simulator mode**: instead of
reading from a physical camera, the node reads frames from an upstream ROS topic published by a
simulator (e.g. Gazebo with a virtual VSSS field).

```cpp
image_subscription = this->create_subscription<sensor_msgs::msg::Image>(
    "/vision/sim_image", 10,
    std::bind(&RosHandler::image_callback, this, std::placeholders::_1)
);

void RosHandler::image_callback(sensor_msgs::msg::Image::SharedPtr msg) {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    latest_frame_ = cv_bridge::toCvCopy(msg, "bgr8")->image;
}
```

The callback is short on purpose, it converts the ROS `Image` to a `cv::Mat` via `cv_bridge` and
stores it in `latest_frame_` under a mutex. The main loop pulls the latest frame via
`get_latest_frame()` (which clones under the mutex) when `app_data.simulator` is true.

The `keep_last(10)` queue means up to 10 frames can buffer if the main loop falls behind, but in
practice the loop is faster than the simulator publishes, so the buffer stays empty and the latest
frame wins.

The mutex is necessary because the subscription callback fires on the executor thread (during
`spin_some()`), while the main loop reads on its own thread. Without it, the `cv::Mat` copy would
race.

## ROS parameters

Six parameters are declared at startup, then read once into local state. The launch file
(`vsss_bringup/launch/vision.launch.py`) sets these:

| Parameter        | Type    | Effect |
|------------------|---------|--------|
| `use_camera`     | `bool`  | If true, open the V4L2 camera at `camera_id`. |
| `simulator`      | `bool`  | If true, subscribe to `/vision/sim_image` instead of a camera. |
| `record_video`   | `bool`  | If true, write `raw_<timestamp>.avi` and `processed_<timestamp>.avi` into `$VSSS_SAVE_PATH`. |
| `debug_mode`     | `bool`  | If true, show all the debug OpenCV windows (mask, label map, Kalman view). |
| `camera_id`      | `string`| Path to the camera device (default `/dev/vsss_cam`). |
| `file_path`      | `string`| If neither `use_camera` nor `simulator` is true, play back this video file in a loop. |

These are read **once at startup** into local variables. They are *not* live, changing them while
the node runs has no effect. To change a parameter, restart the node. The exception is `debug_mode`,
which is read each frame inside the detector via `ros_handler->get_parameter("debug_mode").as_bool()`,
so it can be toggled at runtime via the ROS 2 parameter API.

## Frame loop integration

To bring it all together, here's where each ROS interaction sits in the per-frame loop:

```
1. cap >> image                                  ← camera (or)
   image = ros_handler->get_latest_frame()        ← simulator subscription
2. raw_writer.write(image)                       ← if recording
3. gui.upload_frame(image)
4. detections = detector.update()
5. tracker.update(detections.first, detections.second)
6. tracker.display_debug_image(...)
7. interface_manager.draw_interface()
8. processed_writer.write(processed_frame)        ← if recording
9. gui.display_frame()
10. ros_handler->publish_objects()                ← throttled, all four publishers
11. executor.spin_some()                          ← service callbacks, image subscription
12. cv::waitKey(delay)                            ← input dispatch
```

ROS interaction is concentrated in steps 1, 10, and 11, three points per frame. Everything else is
local. This makes timing easy to reason about: a slow-running pipeline doesn't hold up ROS-internal
scheduling, and a slow ROS callback (e.g. a service request taking a long time) doesn't hold up the
frame loop unless `spin_some` runs over a single frame's budget.

## Why the ROS layer is so thin

Looking at `ros_handler.cpp` as a whole, it's almost entirely *plumbing*: extract data from the
tracker, fill out the corresponding ROS message, publish. There's no logic. No filtering. No fusion
with other sensors. No safety checks beyond "does this robot ID exist?".

This is by design. The vision system's job is to produce reliable position estimates; the ROS layer
exists to broadcast them. Putting logic into the ROS layer, say, dropping messages where the Kalman
estimate has high uncertainty, or fusing with another vision system, would couple the publication
behavior to the tracking behavior, and make both harder to reason about independently.

If a future change wants to filter or fuse before publication, the cleanest place is between the
tracker and the publisher: a `Tracker::get_publishable_state()` that returns a filtered view. The
ROS handler stays thin.

## Summary

| Channel | Direction | Type | Notes |
|---------|-----------|------|-------|
| `/vision/detection` | publish | `EntityState` | Per-entity state, ~15 Hz throttled (effectively per-frame today). |
| `/tf` | broadcast | TF | One frame per entity, parent = `field`. |
| `/vision/markers` | publish | `MarkerArray` | RViz visualization including the field outline. |
| `/vision/prediction` | service | `Prediction` | Linear extrapolation `pos + vel * dt`. |
| `/vision/sim_image` | subscribe | `Image` | Simulator-mode frames. |

The integration layer is intentionally **flat and dumb**: it converts between in-memory tracker
state and ROS messages, nothing more. Anyone integrating with the vision system reads the
EntityState topic for live data, queries the prediction service when planning ahead, and looks up TF
frames when composing geometric queries.
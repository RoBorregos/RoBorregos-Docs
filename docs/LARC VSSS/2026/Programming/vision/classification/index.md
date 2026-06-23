# Classification

The identification stage produced a label map: a per-pixel image where each pixel is either a calibrated
color or `NONE`. **Classification** is the step that turns those pixels into actual *things*, concrete
robots with IDs, positions, and orientations, plus the ball.

In other words: identification answers *"what color is this pixel?"*. Classification answers *"which
robot is that?"*.

## Two identification marks, two algorithms

The robots in the field carry an identification mark on top, and there are **two different mark systems**
the project supports, used in different competitions:

- **ArUco markers** (TMR, Mexican Robotics Tournament). Each robot has a square fiducial tag printed on
  top, encoding its ID as a binary pattern.
- **Color patches** (LARC, Latin American Robotics Competition). Each robot has a small arrangement of
  colored circles on top: one *parent* patch (team color, blue or yellow) and two *children* patches
  (combinations of red, green, cyan, and magenta) that together encode the robot's identity.

Everything *upstream* of classification is shared between the two: same camera, same preprocessing, same
blob mask, same KNC classifier. The label map produced by identification is the same in both cases. What
changes is **how that label map gets read into robots**.

That difference is significant enough that the two systems use entirely different algorithms here, and it
isn't a quality tradeoff, both approaches are equally valid. They solve different problems:

- ArUco is **engineered for identification**. The tag itself encodes the ID directly, with built-in error
  correction. There's no ambiguity about which robot is which: the marker either decodes or it doesn't.
- Color patches are **engineered for visibility**. They're cheaper to manufacture, easier to spot in low
  resolution, and survive partial occlusion better, three colored dots remain recognizable even when an
  ArUco tag would already be unreadable. The tradeoff is that the identity has to be *reconstructed*
  from the spatial arrangement of the patches, which is where the bulk of the classification logic lives.

The choice between them is dictated by the rules of each competition, not by which algorithm is "better".
Both are valid solutions to the same general problem, with different cost profiles.

## ArUco classification (TMR)

Classification with ArUco is mostly delegated to OpenCV's built-in detector. The pipeline:

1. **Pre-processing for the detector.** The frame is upscaled by 1.5× with cubic interpolation, converted
   to grayscale, and sharpened (Gaussian blur subtracted from the original via `addWeighted` with weights
   `3.5 / -1.5`). The upscale gives the detector more pixels per tag corner; the sharpen restores the
   edges that the bicubic resize softens.
2. **Marker detection.** OpenCV's `cv::aruco::ArucoDetector` is run with the `DICT_ARUCO_ORIGINAL`
   dictionary, with detector parameters tuned for the small, possibly-tilted tags worn by the robots
   (low `minMarkerPerimeterRate`, AprilTag-style corner refinement, generous `errorCorrectionRate`).
3. **Identity lookup.** Each detected marker comes with an integer ID. That ID is mapped to a robot ID
   through a lookup table (`RobotIdentities::get_robot_id_by_aruco`).
4. **Pose extraction.** The center is the average of the four corners (after dividing by the resize
   factor to return to the original image's coordinate system). The facing angle is the angle of the
   vector from corner 0 to corner 1 (the top edge of the tag).

The whole flow is one pass over the detector's output. There is no patch grouping, no geometric
reasoning, no edge cases, if the tag is visible and decodes, the robot is identified.

This is why the rest of this document, and most of the actual classification code, is about the
color-patch case. The ArUco branch is short by design.

## Color-patch classification (LARC)

Color patches are where most of the work is. The label map needs to be turned into a list of patches,
the patches need to be grouped into robots, the parent and children of each robot need to be identified,
and from that arrangement the robot's ID and facing have to be reconstructed.

The flow inside `Detector::update` is:

```
label map ──► get_patches ──► get_ball_patch ──► get_robot_patches ──► get_robot_data
                  │                │                    │                     │
              find pixel      isolate the          group patches        compute center,
              regions per     orange/red ball      into robots          facing, ID
                color
```

Each step is described below.

### 1. From label map to patches

`get_patches` takes the label map and runs, for each color from 1 to 7:

- `cv::compare(label_map, color, mask, CMP_EQ)` to extract a binary mask of just that color.
- `cv::findContours(... RETR_EXTERNAL, CHAIN_APPROX_SIMPLE)` to find connected blobs.
- For each contour with area greater than `PATCH_MIN_AREA = 10`, compute the centroid via image moments
  and the bounding box, and record a `Patch` with its color, centroid, bounding box, and a `parent` flag
  set to true if the color is BLUE or YELLOW (the team colors used as the central patch on top of every
  robot).

This is also where one of the more practical edge cases gets handled: **blue-cyan overlap**. Cyan and
blue are close in HSV, and pixels at the border of a blue patch sometimes leak into a "cyan" contour
right next to it. The function checks every blue/cyan pair: if their bounding boxes overlap, and the
intersection covers more than 70% of the cyan box, the cyan patch is dropped and its centroid is
absorbed into the blue patch's centroid (averaged with it). This prevents a phantom cyan patch from
later getting attached to the wrong robot during grouping.

### 2. The ball

The ball is one orange patch, but orange and red are only weakly distinguishable in HSV, so this needs
care. `get_ball_patch` runs first because the ball must be removed from the patch list before robot
grouping, otherwise the robot grouper would try to attach it as a child.

The logic:

1. **Easy case.** Exactly one orange patch in the frame → that's the ball.
2. **Fallback case (no orange).** Re-label every orange patch as red, then look for a red patch that is
   *isolated*, no other patch within `MINIMUM_DISTANCE = 20` pixels. An isolated red patch can't be a
   robot's child (children are always near a parent), so by elimination it's the ball.

The fallback is explicitly marked *"Program Panic"* in the code comments. It only triggers when KNC
fails to distinguish orange from red on a particular frame, which happens occasionally under bad
lighting. It's a recovery path, not a normal flow.

### 3. Grouping patches into robots

Once the ball is out of the way, the remaining patches need to be assembled into robots. Every robot
contributes exactly 3 patches: 1 parent (blue or yellow) and 2 children. The challenge is that on a
busy field, several robots are close to each other, and just looking at "patches near each parent"
doesn't always give a clean answer.

The pipeline handles this in two passes: first the easy robots, then the hard ones.

**Pass 1, `get_isolated_robot_patches`.** For each parent patch, find children within `MINIMUM_DISTANCE`.
If exactly 2 children are found, *and* none of those children is also within `MINIMUM_DISTANCE` of any
other patch in the frame, the parent and its 2 children form an unambiguous robot. These get committed
immediately.

**Pass 2, `get_clustered_robot_patches`.** Whatever didn't resolve cleanly in pass 1 is "clustered",
the patches are tangled enough that simple distance-to-parent isn't enough. The clustered set is solved
by a backtracking search:

- Pick a parent.
- Find candidate child-1 patches near it (excluding pairings already tried, tracked in `relationship`).
- Find candidate child-2 patches near the parent (excluding the already-chosen child-1 and any already-tried triples).
- If both children are found, commit the robot, remove the three patches from the working set, and
  continue.
- If a step fails, record the failed pairing in `relationship`, reset, and try again.
- A hard cap of `MAX_ITERATIONS = 1000` prevents infinite loops on pathological frames.

There's also a special case for "1 parent + 3 children" (one robot in the cluster, plus a stray red
patch that's actually the ball even though step 2 failed to identify it). This is handled in
`get_single_robot_with_ball`, which uses geometric reasoning over the three children to figure out
which red patch is part of the robot and which is the misidentified ball.

### 4. Robot center, facing, and ID

Once a robot has a parent and two children, `get_robot_data` extracts the actual usable information.

**Center.** The geometric median of the 3 patch centroids, computed by Weiszfeld's algorithm (5
iterations of distance-weighted averaging starting from the arithmetic mean). The median is more robust
than the mean to one of the children being mis-located.

**Facing.** Determined by which child sits on the *left* and which on the *right* of the robot, viewed
from above. The code computes the cross product between the parent→midpoint vector and the
parent→child[0] vector to break the tie, then takes `atan2` of the left→right vector to get the
heading angle.

**ID.** The robot's identity is encoded by the *combination* of (parent color, left child color, right
child color). The order matters, `(YELLOW, RED, GREEN)` and `(YELLOW, GREEN, RED)` are different
robots. Once left and right are settled by the cross-product step, the triple is looked up in
`RobotIdentities::DATABASE`, which holds 20 robot signatures (10 yellow + 10 blue) plus one entry for
the ball.

This is where the color-patch system really pays for its added complexity: from 4 child colors and 2
team colors arranged as (parent, left, right), the system encodes 20 unique robot identities with
purely visual features.

## Output

The result of classification is exactly what `Detector::update` returns:

```cpp
std::pair<std::vector<RobotPatch>, std::optional<BallPatch>>
```

A vector of robots, each with an ID, center, facing, and the underlying patches; and an optional ball
position. From here, this data leaves the vision pipeline proper and moves on to **tracking**, which
takes pixel coordinates and turns them into real-world field coordinates with predicted motion over
time.
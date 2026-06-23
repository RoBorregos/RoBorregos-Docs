# Color-patch classification

The color-patch branch is the **LARC** classification path. Every robot wears a small arrangement of
colored circles on top: one *parent* patch in the team color (BLUE for one team, YELLOW for the other)
and two *children* patches drawn from {RED, GREEN, CYAN, MAGENTA}. The combination of parent + ordered
pair of children encodes the robot's identity (20 possible robots: 10 yellow + 10 blue). The ball is a
single ORANGE patch, separate from any robot.

Unlike the ArUco branch, **none of this is delegated to a library**. The label map produced by
identification has no notion of "robot", it's just colored pixels. Everything from "find the connected
regions" through "this triple of patches is robot 14" has to be built from scratch.

That extra distance is where the edge cases live. Most of this document is about them.

## The encoding scheme

Robot identity is determined by three things:

| Slot         | Possible colors                          | Role |
|--------------|------------------------------------------|------|
| Parent       | BLUE, YELLOW                             | Team / center patch |
| Left child   | RED, GREEN, CYAN, MAGENTA                | First identity slot |
| Right child  | RED, GREEN, CYAN, MAGENTA                | Second identity slot |

The *left/right* distinction is critical: `(YELLOW, RED, GREEN)` and `(YELLOW, GREEN, RED)` are two
different robots. This is enforced via a database lookup in `RobotIdentities::DATABASE` (20 robot
signatures + 1 ball entry, totalling 21).

The two children are **physically distinguishable from the parent**: parents are bigger circles, drawn
in vivid blue or yellow, sitting at the geometric center of the marker layout. Children are smaller and
sit on either side of the parent at fixed distances. The classifier exploits this geometry: a *parent*
patch sits at the center of a robot, and its two children sit *near* it.

The `parent` flag of each `Patch` is set by color, not by geometry:

```cpp
p->parent = (color == Color_ID::BLUE || color == Color_ID::YELLOW);
```

So the algorithm trusts the calibration: any blue or yellow patch found in the frame is treated as a
candidate robot center. If KNC mis-classifies a non-parent pixel cluster as blue, the grouping logic is
expected to reject it later when no plausible children can be attached to it.

## The frame journey

```
label map
    │
    ▼
get_patches             (label_map → list of Patch)
    │
    ▼
get_ball_patch          (find the orange ball; remove it from patches)
    │
    ▼
get_robot_patches
    ├── get_isolated_robot_patches      (easy robots, clean parent+2 children)
    └── get_clustered_robot_patches     (hard robots, overlapping clusters)
        └── get_single_robot_with_ball  (edge case: 1 robot + ball stuck together)
    │
    ▼
get_robot_data          (compute center, facing, ID for each robot)
    │
    ▼
{robots, ball}
```

Each step is documented below, and so is what it does when the frame is *not* in a clean state.

## 1. From label map to patches

`get_patches` is a per-color contour pass:

```cpp
for (int color = 1; color <= 7; color++) {
    cv::Mat mask;
    cv::compare(label_map, color, mask, cv::CMP_EQ);          // binary mask of just this color

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& cnt : contours) {
        if (cv::contourArea(cnt) > PATCH_MIN_AREA) {          // filter tiny noise
            const cv::Moments m = cv::moments(cnt);
            if (m.m00 == 0) continue;                          // degenerate contour, skip

            // record the patch
        }
    }
}
```

For each color (1 = BLUE through 7 = ORANGE), the label map is thresholded to a binary image,
`findContours` extracts the connected regions, and each region with area above `PATCH_MIN_AREA = 10` is
turned into a `Patch`. The patch records:

- `id`, a sequential integer assigned across all colors (used as a stable handle in later passes).
- `color`, the color ID.
- `centroid`, computed from image moments: `(m10/m00, m01/m00)`.
- `bounding_box`, `cv::boundingRect(cnt)`, used for one of the edge cases below.
- `parent`, true if the color is BLUE or YELLOW.
- `valid`, starts true, can be flipped to false during cleanup.

`RETR_EXTERNAL` is intentional: holes inside a patch (which can happen when KNC labels a single pixel as
NONE in the middle of an otherwise solid region) are ignored. A patch with a 1-pixel hole in it is
still one patch.

### Edge case: blue/cyan bounding-box overlap

This is one of the trickier failure modes of the upstream pipeline, and it gets handled here.

**The problem.** Cyan and blue are HSV neighbors. The KNC classifier uses one center per calibration
sample, and even with 5-vote majority filtering, pixels near the *boundary* of a blue patch sometimes
get labeled as cyan. The Moore neighborhood filter cleans most of these up, but not all, and what
survives is usually a small "cyan halo" sitting *right next to* a blue patch.

**Why it matters.** That phantom cyan patch then enters the patch list as a normal `Patch`. During robot
grouping, it might be close enough to a parent to be picked up as a child, leading to a robot that
*looks* like it's been correctly identified but has the wrong ID (because cyan is one of the legitimate
child colors).

**The fix.** After all patches are collected, every blue patch is checked against every cyan patch:

```cpp
cv::Rect intersection = blue->bounding_box & cyan->bounding_box;
if (intersection.area() > 0) {
    float coverage = (float)intersection.area() / (float)cyan->bounding_box.area();
    if (coverage > 0.7f) {
        cyan->valid = false;
        blue->centroid = (blue->centroid + cyan->centroid) * 0.5;
    }
}
```

If a cyan patch's bounding box is *more than 70% covered* by a blue patch's bounding box, the cyan
patch is judged to be a halo of the blue one. It's marked invalid (and removed at the end of the
function), and the blue patch's centroid is shifted to the average of its own and the cyan's centroids
— absorbing the leaked pixels back into the blue patch geometrically.

The 70% threshold is a tradeoff. Lower would absorb legitimate cyan child patches that happen to sit
near a blue parent (false positive, losing a real cyan). Higher would let through halos that don't
fully overlap (false negative, keeping a phantom). 70% works because real children are *adjacent to*
parents, not *inside* them, so the bounding-box overlap is rarely above ~30%.

## 2. The ball

`get_ball_patch` runs **before** robot grouping for a reason: the ball is one of the patches in the
list, and if it isn't pulled out, the robot grouper will try to attach it as a child of some nearby
parent. So the ball has to be extracted first and removed from `patches`.

There's exactly one ball, and it should be ORANGE. In a clean frame:

```cpp
if (orange_occurrences == 1 && last_orange) {
    // that's the ball
}
```

Done.

### Edge case: orange / red collapse

Orange and red are HSV neighbors and KNC sometimes can't tell them apart on a given frame. When that
happens, the ball gets labeled RED instead of ORANGE, and *vanishes from the orange count*. The code
calls this state a **"Program Panic"** in its comment, because the assumption "ball = unique orange
patch" has just failed.

The recovery is a fallback heuristic:

```cpp
// Re-label every remaining orange patch as red (collapse the ambiguity)
for (const auto& p : patches) {
    if (p->color == Color_ID::ORANGE) p->color = Color_ID::RED;
}

// Find a red patch with no other patch within MINIMUM_DISTANCE
for (const auto& p1 : patches) {
    if (p1->color != Color_ID::RED) continue;

    bool is_isolated = true;
    for (const auto& p2 : patches) {
        if (p1->id == p2->id) continue;
        if (distance(p1->centroid, p2->centroid) <= MINIMUM_DISTANCE) {
            is_isolated = false; break;
        }
    }

    if (is_isolated) {
        // that's the ball
    }
}
```

The reasoning: red patches that are part of a robot are children, and children always sit *near* their
parent (within `MINIMUM_DISTANCE = 20` pixels). A red patch with no other patch within that radius
*can't* be a robot child, it has to be the ball.

This works as long as no red robot child happens to be far from any other patch (which only happens if
the rest of the robot is missing, in which case the frame is broken anyway). If it fails, for instance,
two isolated red patches because two robots are partially occluded, the algorithm picks the first one
it finds and accepts the resulting mis-identification on this frame. The next frame will probably have
better data.

Once the ball is identified, the corresponding patch is removed from `patches` in `Detector::update`:

```cpp
if (ball_patch.has_value()) {
    int target_id = ball_patch.value().patch->id;
    patches.erase(...);  // remove ball patch from the working list
}
```

So when robot grouping starts, the ball is no longer a candidate for anything.

## 3. Grouping patches into robots

This is the core of the color-patch classification. Every robot needs to be built from exactly 1 parent
+ 2 children, and the assignment must be unique (no patch can be claimed by two robots).

A naive approach, "for every parent, take the two closest children", fails as soon as two robots get
close to each other on the field. The neighborhoods overlap; a single child might be within range of
two parents. There's no way to resolve the ambiguity by looking at one parent in isolation.

The pipeline solves this in two passes: easy first, hard second.

### Pass 1, `get_isolated_robot_patches`

For each parent, collect every child patch within `MINIMUM_DISTANCE = 20` pixels. If exactly **2**
children are found, *and* none of those children is also within `MINIMUM_DISTANCE` of any other patch
in the frame, the robot is unambiguous. Commit it immediately.

```cpp
if (candidates.size() == 2) {
    bool accept_candidates = true;
    for (const auto& child : candidates) {
        for (const auto& patch : patches) {
            // skip the parent and the two candidates themselves
            if (patch->id == parent->id || patch->id == candidates[0]->id || patch->id == candidates[1]->id) continue;
            if (distance(child->centroid, patch->centroid) <= MINIMUM_DISTANCE) accept_candidates = false;
        }
    }
    if (accept_candidates) { /* commit robot */ }
}
```

This pass picks up any robot that's far enough from all the others to be visibly alone. In a typical
match, that's *most* of the robots, *most* of the time.

The result is added to `robot_patches`, but the underlying patches aren't removed from `patches` yet,
that happens in pass 2 via `set_difference`.

### Pass 2, `get_clustered_robot_patches`

What's left after pass 1 is the *clustered* robots: robots whose neighborhoods overlap with another
robot's. The clustered set is solved by **backtracking search**.

The algorithm maintains:

- `clustered_patches`: the working set of unassigned patches in the cluster.
- `relationship`: a list of `RobotRelationship` records `{parent_id, child_1_id, child_2_id?}` that
  records every (parent, child_1) and (parent, child_1, child_2) triple already attempted, so the
  search doesn't try the same assignment twice.

The main loop:

```cpp
while (!clustered_patches.empty()) {
    if (++iterations > MAX_ITERATIONS) return;       // safety bail-out

    // Split the working set into parents and children.
    // Sanity check: 2 children per parent is required.
    if ((parents.size() * 2) != children.size()) {
        // Try the special "1 parent + 3 children" fallback (see edge case below)
        return;
    }

    // Pick a parent. Pick a child_1 candidate that hasn't been tried.
    // Pick a child_2 candidate that hasn't been tried with this (parent, child_1).
    // If both succeed: commit the robot, remove its 3 patches, continue.
    // If child_1 fails: undo, retry from the top.
    // If child_2 fails: record the (parent, child_1) attempt as failed and retry.
}
```

The two helpers `find_child_1_candidates` and `find_child_2_candidates` consult `relationship` to
exclude pairings that have already been ruled out, so the search makes progress instead of looping.

The `MAX_ITERATIONS = 1000` cap exists because pathological label maps can produce configurations the
search can't resolve, for example, a parent surrounded by exactly the wrong number of children. In
those cases, the function bails out with a partial result rather than spinning forever.

### Edge case: 1 parent, 3 children (robot stuck to ball)

When a robot is right next to the ball, the cluster ends up with the ball plus the robot. Step 2 (ball
detection) should have caught this and removed the ball patch before grouping starts, but if it
didn't (e.g., because the orange-as-red panic was triggered and the ball wasn't isolated enough to find
via the fallback), the cluster contains:

- 1 parent
- 2 robot children
- 1 stray red patch (the actual ball, currently mis-labeled)

Total: 1 parent + 3 children, which fails the `parents * 2 == children` check.

Instead of giving up, the pipeline calls `get_single_robot_with_ball`, which has to figure out which of
the three children is the ball. It uses the color and geometry of the children. Two cases:

**Case A, exactly 1 of the 3 children is RED.** That red patch is the ball. The other two children
are the robot's actual children (in some other colors). Done.

**Case B, exactly 2 of the 3 children are RED.** Now both reds are candidates: one is the robot's red
child, one is the ball. They have to be distinguished by geometry.

The third child (call it `X`, in some non-red color) is a robot child for sure. The robot's other child
must be one of the two reds. Two possible layouts:

```
"Consecutive":   X, R, R           "Intermediate":   R, X, R
                     P                                      P
```

In the **consecutive** layout, the ball is the red farther from `X` (since the robot's red child sits
between `X` and the ball in the line of children, so the further red is "outside" the robot).

In the **intermediate** layout, both reds are equally far from `X`, and the centroid alone *cannot*
distinguish which red is the robot child and which is the ball. The code makes a best-guess pick (the
*farther* red is the ball) and commits, same logic as case A. This is acknowledged in the code comment:

```cpp
// In the intermediate there is no easy way to know using only centroids.
// See flaws of using only centroids in the documentation
```

This is one of the rare places where the algorithm can produce a wrong answer for a single frame, and
the design accepts it. The error is rare (it requires a robot to be touching the ball *and* the ball
to have been mis-classified as red), self-correcting (the next frame, with a slightly different
geometry, usually resolves correctly), and downstream tracking smooths over a single bad frame.

### Edge case: parent without children

If `find_child_1_candidates` returns empty, the parent has no available children. The search resets,
clears the candidate robot list, and continues, meaning that parent will be retried later, possibly in
a state where other patches have been removed. If it never succeeds, it eventually gets ejected when
the iteration cap fires. The parent is silently dropped on this frame; tracking will keep the previous
position based on the prior frame's data.

### Edge case: too few or too many patches in the cluster

If `parents.size() * 2 != children.size()`:

- `0 parents, N children` → return immediately (no robots to build).
- `1 parent, 3 children` → handed to `get_single_robot_with_ball` (above).
- Anything else → return. The cluster is broken in some way that the algorithm doesn't try to recover
  from. This frame contributes whatever was committed in pass 1; the rest is dropped.

This is intentional. The space of "broken cluster" configurations is large, and trying to handle every
one of them invites bugs. Returning on bad input lets tracking carry the previous frame's robots
forward by inertia, which is more reliable than guessing.

## 4. Robot center, facing, and ID

Once a robot has its three patches, `get_robot_data` extracts the actual usable values.

### Center: geometric median (Weiszfeld)

The robot's center is the **geometric median** of the three patch centroids, the point that minimizes
the sum of distances to all three. For three points, this is robust to one of them being slightly
mis-located: a 5-pixel error in one centroid moves the median by less than the mean would move.

The implementation is **Weiszfeld's algorithm**: start at the arithmetic mean, then iteratively
re-weight each point by the inverse of its distance to the current estimate.

```cpp
for (int i = 0; i < iterations; ++i) {
    double numX = 0, numY = 0, den = 0;
    for (const auto& p : points) {
        const double dist = cv::norm(p - median);
        if (dist < 0.1f) continue;                 // avoid division by ~0
        const double weight = 1.0 / dist;
        numX += p.x * weight;
        numY += p.y * weight;
        den += weight;
    }
    if (den != 0) { median.x = numX / den; median.y = numY / den; }
}
```

Five iterations is enough for the algorithm to converge in this regime (3 points sitting within ~30
pixels of each other).

The `dist < 0.1` skip handles the case where the running estimate is nearly on top of one of the input
points, which would produce a divide-by-zero. The point is excluded from the iteration but reconsidered
on the next one once the estimate has moved.

### Facing: cross-product disambiguation

Facing is the heading direction of the robot in the image plane, expressed as an angle in radians.

The naive idea would be "atan2 of (parent → midpoint of children)". This works if you know which
direction is "front", but the children are *unordered* in `robot.children`, the grouping algorithm
doesn't care which one is left and which is right.

The fix: use a **cross-product sign check** to decide which child is the left one and which is the
right one (as seen from above), then take `atan2` of the left → right vector and apply the convention
that the robot's "front" is 90° clockwise from that.

```cpp
double cross = forward_temp.x * v0.y - forward_temp.y * v0.x;

if (cross > 0) {
    p_left  = robot_patch.children[0]->centroid;
    p_right = robot_patch.children[1]->centroid;
} else {
    p_left  = robot_patch.children[1]->centroid;
    p_right = robot_patch.children[0]->centroid;
}

double dx_LR = p_right.x - p_left.x;
double dy_LR = p_right.y - p_left.y;
robot_patch.facing = std::atan2(dx_LR, -dy_LR);
```

The sign of the cross product tells which side of the parent→midpoint vector each child is on. The
final `atan2(dx_LR, -dy_LR)` gives the angle of the perpendicular to the left→right vector (a 90°
rotation), which by the layout convention is the front of the robot.

### ID lookup

The ID is determined by `(parent color, left child color, right child color)`:

```cpp
robot_patch.id = RobotIdentities::get_id(
    static_cast<unsigned char>(robot_patch.parent->color),
    left_color,
    right_color
);
```

`RobotIdentities::DATABASE` is a 21-entry table holding every legal (parent, c1, c2) triple. If the
exact triple is found, the matching robot ID is returned; if not, `-1` is returned (unknown robot).

The order matters here: `(YELLOW, RED, GREEN)` (robot 0) and `(YELLOW, GREEN, RED)` (robot 2) are
different robots. This is exactly what the cross-product disambiguation in the facing step is for,
without a reliable left/right assignment, the ID would be unstable.

A robot whose ID resolves to `-1` is still a valid `RobotPatch` with a center and facing; it just isn't
recognized as one of the 20 known robots. Downstream consumers can filter by `id != -1`.

## Output

The output of color-patch classification, just like the ArUco branch, is:

```cpp
std::pair<std::vector<RobotPatch>, std::optional<BallPatch>>
```

Each `RobotPatch` carries a known ID (or `-1`), a sub-pixel center, a heading angle, and pointers back
to the underlying patches. The ball, if present, carries its center as well. Both branches feed into
the same downstream tracking stage, which is what finally turns these pixel-space coordinates into
real-world field coordinates.

## Edge case summary

For quick reference, the failure modes the algorithm explicitly handles:

| Edge case                                  | Where           | Resolution |
|--------------------------------------------|-----------------|------------|
| Cyan halo on a blue patch                  | `get_patches`   | Bounding-box overlap > 70% → cyan dropped, blue centroid averaged in |
| Degenerate contour (`m00 == 0`)            | `get_patches`   | Skipped silently |
| No orange patch (ball mis-labeled as red)  | `get_ball_patch`| Find isolated red patch (no neighbor within 20 px); declare it the ball |
| Multiple orange patches                    | `get_ball_patch`| Falls through to the isolated-red fallback (ambiguous; first match wins) |
| Robot adjacent to ball, ball mis-labeled   | `get_single_robot_with_ball` | Geometric reasoning over 3 children: identify the outlier red as the ball |
| Two reds in 1-parent/3-children case ("intermediate" layout) | `get_single_robot_with_ball` | Best-guess pick; accepts occasional mis-ID for one frame |
| Parent with no available children          | `get_clustered_robot_patches` | Skipped on this frame; tracking carries previous position |
| Cluster fails the parent/child count check | `get_clustered_robot_patches` | Function returns; cluster contributes nothing this frame |
| Backtracking search loops                  | `get_clustered_robot_patches` | Hard-cap at `MAX_ITERATIONS = 1000` |
| Geometric median lands on a centroid       | `find_geometric_median` | Inverse-distance term skipped (`dist < 0.1`) |
| Robot ID triple not in database            | `get_robot_data`| ID set to `-1`; robot kept but tagged as unknown |

The recurring strategy: **fail soft and let tracking absorb the gap**. A robot that disappears for one
frame is invisible at the controller for ~16ms; a robot that gets *misidentified* every frame produces
incoherent paths. So when in doubt, the code drops the frame's contribution rather than fabricating
one.
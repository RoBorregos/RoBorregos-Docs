# ArUco classification

The ArUco branch of classification is used in environments where the robots wear printed fiducial markers
on top, concretely, the **Mexican Robotics Tournament (TMR)**. It's the simpler of the two classification
algorithms in the project, but "simpler" here means *most of the work has been moved into a dedicated,
well-studied library* (OpenCV's `cv::aruco`), not that the code can be careless. The detector still has to
be tuned for the specific size, motion, and lighting of the field, and the surrounding pre/post-processing
has a real effect on detection rate.

This document walks through every piece of `Detector::detect_aruco_robots`.

## What is an ArUco marker

An ArUco marker is a square, black-and-white binary pattern with a thick black border. The interior is a
grid of black/white cells (typically 5×5 or 6×6) that encodes:

- An **ID** in the dictionary (an integer).
- **Error-correction bits** that let the decoder recover the ID even if a few cells are misread.

Detection works by finding the black border (a square contour with the right aspect ratio), warping the
inside to a canonical view, thresholding it into a bit grid, and decoding the bits against the chosen
dictionary. Because the decoder knows the four corners of the square, it also gives back the marker's
**pose in the image**: the (x, y) of each corner, in image pixels.

That last point is what makes ArUco useful here: detection delivers identity *and* orientation in a single
pass, with no extra reasoning needed. Compare with the color-patch branch, which has to reconstruct the
robot's facing from the spatial arrangement of three colored patches.

## Frame journey

ArUco runs **after** identification, in `Detector::update`, but it does not consume the label map. It
operates on the original BGR image (after preprocessing). The full sequence inside `update` is:

```
preprocessed BGR frame ──► (identification produces label_map, used by ball logic)
                       └─► download to CPU ──► detect_aruco_robots
                                                       │
                                                       ▼
                                                 robot_patches
```

The two branches are independent: ArUco doesn't see the label map, and the patch/ball logic doesn't see
the ArUco results. They both write into the same `robot_patches` vector at the end.

## The algorithm step by step

### 1. Frame preparation

Three transformations are applied to the BGR frame before it reaches the detector:

```cpp
cv::resize(frame, enlarged, cv::Size(), 1.5, 1.5, cv::INTER_CUBIC);
cv::cvtColor(enlarged, gray, cv::COLOR_BGR2GRAY);
cv::GaussianBlur(gray, sharp, cv::Size(0, 0), 3);
cv::addWeighted(gray, 3.5, sharp, -1.5, 0, gray);
```

**Upscale by 1.5×.** A small ArUco marker viewed from above on a VSSS field can be only ~20×20 pixels at
the source resolution, barely enough to resolve a 5×5 bit grid. Cubic upscaling pushes that to ~30×30,
giving the detector more pixels per cell and substantially raising the decode rate. The 1.5 factor is a
compromise: bigger means more work and slower frames, smaller means missed detections.

**Convert to grayscale.** ArUco is purely a brightness pattern. Color information is irrelevant and
removing it lets the detector skip channel reduction internally.

**Unsharp mask (`addWeighted` with weights 3.5 and -1.5).** This is a textbook unsharp mask:
`output = 3.5 * gray + (-1.5) * blurred_gray`. The blurred copy approximates the low-frequency content
of the image; subtracting a scaled version of it amplifies high frequencies, sharpening edges. Cubic
upscaling tends to soften edges, so this restores the crisp transitions that ArUco's contour finder
needs to lock onto the marker borders. The coefficients `3.5 / -1.5` sum to `2.0`, which roughly doubles
the contrast as a side effect, useful in dim VSSS lighting, but it can blow out highlights if the
original frame was already bright.

The intermediate result is shown in the `grey+enlarged+sharp` debug window, so the operator can confirm
that the pre-processed image looks clean before blaming the detector for missed markers.

### 2. Detector parameters

The detector itself is constructed with a tuned `DetectorParameters` object. Each parameter is chosen for
the conditions the project actually faces: small markers, moderate motion blur, dim/uneven lighting, and
camera tilts of up to ~10° from the field plane. Going through them in groups:

**Adaptive thresholding window.** ArUco binarizes the image with adaptive thresholding (per-pixel
threshold computed from a local window).

```cpp
adaptiveThreshWinSizeMin  = 3
adaptiveThreshWinSizeMax  = 23
adaptiveThreshWinSizeStep = 5
```

This sweeps window sizes 3, 8, 13, 18, 23, running detection once per window. Multiple sizes catch
markers at different scales and under different local contrast: a 3-pixel window finds tiny markers in
high-contrast spots, a 23-pixel window finds large markers in regions with smooth gradients. The cost
is proportional, five passes, but is acceptable because the per-pass work is small once the image is
preprocessed.

**Marker size floor.**

```cpp
minMarkerPerimeterRate = 0.015
```

This is the minimum perimeter a candidate quadrilateral must have, expressed as a fraction of the *image
dimensions*. The default is around 0.03; lowering it to 0.015 means markers as small as ~1.5% of the
frame's max dimension can still be detected. On a 1080p frame, that's a marker as small as ~30 pixels on
a side, exactly the regime VSSS robots produce.

**Error correction.**

```cpp
errorCorrectionRate    = 0.6
maxErroneousBitsInBorderRate = 0.5
minOtsuStdDev          = 3.0
```

`errorCorrectionRate = 0.6` lets the decoder use up to 60% of the dictionary's error-correction budget
per marker, which is generous. The trade is between false negatives (the marker is real but not decoded)
and false positives (random noise gets decoded as a valid ID). The high rate is paired with the strict
identity table downstream (only specific ArUco IDs are mapped to robots; everything else is dropped),
so the false-positive risk is contained.

`maxErroneousBitsInBorderRate = 0.5` allows up to half of the *border bits* (which should all be black)
to be mis-thresholded before the marker is rejected. This is again generous, accounting for
black-borders that get partially absorbed into shadows.

`minOtsuStdDev = 3.0` is the minimum standard deviation a candidate region must have before Otsu's
thresholding is even attempted on it. Below this, the region is presumed to be flat/uniform (not a
marker) and discarded. The value of 3.0 is low and admits faint markers.

**Polygon approximation tolerance.**

```cpp
polygonalApproxAccuracyRate = 0.05
```

Marker candidates start as contours, which then get approximated as polygons. This rate controls how
loose the approximation is allowed to be (as a fraction of the contour's perimeter). A higher value
admits more bent/distorted "squares", which helps when the marker is on a robot tilted relative to the
camera but can produce false candidates from cluttered backgrounds.

**Cell sampling.**

```cpp
perspectiveRemoveIgnoredMarginPerCell = 0.25
perspectiveRemovePixelPerCell         = 10
```

After warping the marker to a canonical square, each bit cell needs to be classified as black or white.
`perspectiveRemovePixelPerCell = 10` says each cell is sampled at 10×10 = 100 pixels. With the 0.25
margin-per-cell ignored, only the central 50% of each cell (5×5 pixels) actually contributes to the
black/white decision. The wide ignored margin is what makes the decoder robust to imprecise corner
localization: even if the corners are off by a fraction of a cell, the centers of the cells still sit
inside the right region.

**Corner refinement (the most important block).**

```cpp
cornerRefinementMethod        = CORNER_REFINE_APRILTAG
cornerRefinementWinSize       = 3
cornerRefinementMaxIterations = 30
cornerRefinementMinAccuracy   = 0.01
```

After initial detection, the four corners of the marker are sub-pixel-refined. Three methods are
available in OpenCV; this code uses the **AprilTag** refinement, which is the highest-precision option
(also the slowest). It iteratively fits the marker's edges to lines and locates corners as the
intersections of those lines, instead of just snapping to the nearest pixel maximum.

Why does this matter? The robot's facing angle is computed *directly* from corner positions
(`atan2(corner1.y - corner0.y, corner1.x - corner0.x)`). A 1-pixel error in corner localization on a
30-pixel marker translates to roughly `atan2(1, 30) ≈ 2°` of heading error, significant for a
controller trying to point a robot at a ball. Sub-pixel refinement brings this down by an order of
magnitude.

The 30-iteration cap with 0.01-pixel accuracy threshold is conservative, refinement usually converges
in fewer iterations. The `winSize = 3` is the half-window for sampling intensity gradients during
refinement; small enough to stay inside one bit cell.

### 3. Running the detector

```cpp
cv::aruco::ArucoDetector detector(dictionary, detectorParams);
detector.detectMarkers(gray, markerCorners, markerIds);
```

The dictionary is `DICT_ARUCO_ORIGINAL`, the legacy 1024-marker dictionary that uses 5×5 bit grids.
`markerIds` ends up as a vector of integer IDs (one per detected marker), and `markerCorners` is a
parallel vector of 4-corner lists (each corner is a `cv::Point2f` in the *upscaled* image's coordinate
system).

The two outputs are aligned by index: `markerIds[i]` is the ID of the marker whose corners are in
`markerCorners[i]`.

### 4. Identity lookup

For each detected marker, the ID is mapped to a robot ID via a small lookup table:

```cpp
robot.id = RobotIdentities::get_robot_id_by_aruco(markerIds[i]);
```

The table (`RobotIdentities::DATABASE`) has explicit entries:

| Robot ID | ArUco ID |
|----------|----------|
| 0        | 256      |
| 1        | 272      |
| 2        | 273      |
| 10       | 955      |
| 11       | 771      |
| 12       | 939      |
| 20       | -1 (ball, not detected via ArUco) |

If the detected ArUco ID isn't in the table, `get_robot_id_by_aruco` returns `-1`. The ID is still
written to `robot.id`, so the robot enters the pipeline tagged as "unknown". Downstream consumers can
filter on `id != -1` if they only want known robots.

This is the first line of defense against false positives: even if the detector hallucinates an ArUco ID
from background clutter, it will almost certainly not be one of the 6 IDs in the table, and the result
gets the `-1` sentinel.

### 5. Center

```cpp
cv::Point2f center(0, 0);
for (const auto& corner : markerCorners[i]) center += (corner / resize_factor);
robot.center = center * 0.25f;
```

The marker center is the **average of its 4 corners**. Two things are happening at once here:

- **Down-scaling back to original image coordinates.** The detector operates on the 1.5×-upscaled
  image, so each corner is divided by `1.5` (the `resize_factor`) inside the loop, returning it to the
  original image's coordinate system.
- **Averaging.** After summing 4 corners (each pre-divided by 1.5), multiplying by `0.25` gives the
  arithmetic mean.

Order of operations: each corner is divided by 1.5, the four results are summed, and the sum is
multiplied by 0.25. Algebraically equivalent to "compute mean, then divide by 1.5", and produces the
center in original pixel coordinates.

This center is what tracking will eventually transform into a real-world field coordinate.

### 6. Facing

```cpp
cv::Point2f front_vec = markerCorners[i][1] - markerCorners[i][0];
robot.facing = std::atan2(front_vec.y, front_vec.x);
```

ArUco's corner ordering is fixed by the dictionary specification: corners are listed in clockwise order
starting from the **top-left of the marker as the marker is "supposed" to be oriented**. So
`corners[0] → corners[1]` is the *top edge*, vector pointing right when the marker is upright.

If the marker is rotated by some angle θ around the field plane, that top edge rotates by the same θ.
Taking `atan2` of the edge vector gives θ directly: the heading angle of the marker, in image-frame
radians.

Note that the resize factor cancels here: subtracting two corners and taking `atan2` is invariant to
uniform scaling, so the corners don't need to be divided by 1.5 first.

The convention for the angle is standard image-frame: 0 radians is "marker pointing right in the image",
positive angle rotates clockwise (because image y axis points down). Tracking later remaps this into the
field frame.

## Why this is so much shorter than the color-patch path

The full ArUco function fits in ~60 lines. The color-patch path takes four functions, two passes,
hundreds of lines, and several edge cases. The asymmetry is real and worth understanding:

ArUco does the hard parts inside OpenCV: finding the marker (contour analysis + adaptive thresholding +
polygon approximation), decoding it (perspective warp + cell sampling + bit decoding + error
correction), and refining its pose (sub-pixel corner refinement). All of this is done by a library that
has been written, tuned, and optimized for years specifically for this task. The code in this repo is
just the configuration layer.

Color-patch detection has no equivalent library. There is no `cv::detectColorPatchRobot` that reads a
parent + 2 children and returns a robot ID, that logic *has* to live in our code, because the robot
encoding is project-specific. Both approaches end up at the same destination (a robot with an ID,
center, and facing); ArUco gets there by leveraging an existing primitive, color patches by building one
from scratch.

That's also why the project supports both: ArUco is convenient where the rules allow it (TMR), and
color patches are necessary where they don't (LARC). Neither approach is a workaround for the other.

## Failure modes and how they're handled

Practical things that go wrong with ArUco detection on a VSSS field, and what the current code does
about them:

| Failure                               | Cause                                                      | Mitigation in code |
|---------------------------------------|------------------------------------------------------------|---------------------|
| Marker too small to decode            | Camera too far from field                                  | 1.5× upscale + sharpen |
| Marker found but mis-decoded          | Lighting glare, motion blur                                | Generous error-correction (`0.6`); strict ID lookup table catches the rest |
| Marker not found at all               | Marker occluded, washed out, or below `minMarkerPerimeterRate` | Lowered `minMarkerPerimeterRate` to 0.015; nothing further can be done if the marker isn't visible |
| Heading is unstable from frame to frame | Corner localization noise                                | AprilTag corner refinement, sub-pixel accuracy 0.01 |
| False positive ArUco ID               | Random pattern in background decodes to some ID            | Only IDs in the lookup table become known robots; everything else gets `id = -1` |
| Marker tilted on robot, not flat      | Robot leaning, camera not perfectly orthogonal             | Loose `polygonalApproxAccuracyRate = 0.05`; generous cell-sampling margin |

The recurring theme: **be permissive at detection, strict at identity**. The detector is tuned to
catch as many candidates as possible, accepting some risk of decoding noise. The identity lookup table
then filters down to only the 6 known robot IDs, dropping any false positives before they reach
tracking.
# Blob mask

The blob mask is a coarse binary filter that runs **before** color identification and decides which pixels
are even worth classifying. It does not try to assign a color to anything, it simply answers a yes/no
question for every pixel: *"is this pixel plausibly part of a colored marker?"*

Everything that fails this test is marked as background and skipped by the K-Nearest-Centers classifier
downstream. Everything that passes is forwarded as-is, still with its original HSV value, to be classified.

## Purpose

The classifier is the expensive part of identification. It compares each pixel against every calibrated
color center and runs a K-vote for the top matches. Doing that on the entire frame is wasteful, because
most pixels in any given frame are obviously *not* markers: the field surface, shadows under the robots,
black robot bodies, the area outside the field, the dark frame around the lens.

The blob mask exists to throw all of those away in a single, cheap pass before classification runs.

There is also a more subtle reason. The classifier is a *nearest*-centers classifier with a distance cap:
if a dark gray shadow happens to be slightly closer to one calibrated color than another, it could still
be labeled as that color, because relative distance does not capture "this pixel doesn't really *look*
like any marker". The blob mask sidesteps this by removing dark and unsaturated pixels up front, so the
classifier is only ever asked about pixels that are at least *plausibly* colored.

## How it works

The mask is produced in `Detector::get_blob_mask` with a single OpenCV call:

```cpp
const cv::Scalar lower(p.h_min, p.s_min, p.v_min);
const cv::Scalar upper(p.h_max, p.s_max, p.v_max);
cv::cuda::inRange(hsv_image, lower, upper, mask);
```

That is the entire algorithm. `cv::cuda::inRange` checks each pixel of the HSV image and outputs `255` if
**all three** of its channels fall within the `[lower, upper]` range, or `0` otherwise. The result is a
single-channel binary image the same size as the input, where white pixels are candidates and black
pixels are guaranteed background.

The thresholds come from `MaskParams`, which are six values stored alongside the rest of the calibration:
`h_min`, `h_max`, `s_min`, `s_max`, `v_min`, `v_max`. They are tuned interactively in the
**Mask Calibration** menu, where each one is exposed as a slider. The hue sliders go from 0 to 180 and
the saturation/value sliders from 0 to 255, matching OpenCV's HSV ranges.

## What gets thresholded in practice

In principle all six bounds are configurable. In practice, the calibrations shipped with the project show
a clear pattern, and it is the pattern that matters:

| Channel | Typical lower | Typical upper | What it removes |
|---------|---------------|---------------|-----------------|
| H       | 0             | 180           | nothing, the full hue wheel is kept |
| S       | ~15–30        | 255           | grays, whites, washed-out pixels |
| V       | 0–90          | 255           | shadows, dark surfaces, the black field border |

Hue is essentially never restricted at this stage. That is intentional: hue is what the identification
step uses to actually *tell colors apart*, so cutting hue here would mean rejecting entire colors before
they ever reach the classifier. The blob mask deliberately stays color-agnostic.

What it really filters on is **saturation and value**:

- Cutting low saturation removes anything gray. The field surface, white lines, neutral plastic, and any
  pixel that has been washed out by glare all sit at low saturation regardless of their hue.
- Cutting low value removes anything dark. Robot bodies, shadows, the dark border around the playing
  area, and any region the camera is genuinely failing to expose all sit at low value.

Together these two cuts isolate the only pixels that could plausibly belong to a bright, colored marker.

## When it runs

The blob mask is built every frame, immediately before the classifier:

```
preprocessed HSV frame
        │
        ▼
   get_blob_mask  ──►  binary mask
        │                  │
        └──────────┬───────┘
                   ▼
        launch_color_segmentation
                   │
                   ▼
              label map
```

The classifier kernel takes both the HSV frame and the mask. For each pixel where the mask is `0`, the
kernel writes `Color_ID::NONE` and returns immediately, without touching the calibrated centers at all.
Only mask-`255` pixels enter the actual KNC vote.

## Calibration notes

Because the blob mask is upstream of everything else, mis-tuning it has cascading effects:

- If the thresholds are **too aggressive** (high `s_min` or `v_min`), real marker pixels get rejected at
  the mask stage. The classifier never sees them, the markers come out smaller than they should, and
  blob detection downstream may lose them entirely or produce unstable centroids.
- If the thresholds are **too lax** (low `s_min` and `v_min`), too many background pixels survive into
  the classifier. Performance drops because the GPU spends time on pixels it should have skipped, and
  the classifier may start labeling shadows or dim background regions as colors.

The right tuning is the loosest mask that still cleanly cuts the field surface, the dark robot bodies,
and the area outside the field. The "Mask Calibration" tool exists exactly to find this point: the GUI
shows the mask in real time as the sliders move, so the operator can watch the field disappear into
black while the markers stay white.

A debug overlay (`Mask S+V`) is available when `debug_mode` is on, and shows the live mask the
classifier is actually seeing.
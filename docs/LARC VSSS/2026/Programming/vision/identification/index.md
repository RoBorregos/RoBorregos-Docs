# Identification

Once the frame has been preprocessed and cleaned up, the pipeline still has just an image - a 2D grid of
HSV pixels. **Identification** is the step that turns that image into something semantically meaningful: a
per-pixel map saying *"this pixel is blue, that one is yellow, this one is nothing"*. Everything that comes
after (blob detection, robot assembly, tracking) builds on top of this label map.

In other words: preprocessing makes the colors trustworthy, identification *reads* them.

## Frame journey

By the time identification runs, the frame is already in HSV (the last format produced by the preprocessing
pipeline). From there, every pixel goes through the following journey:

1. The HSV frame and a **blob mask** are received. The blob mask is a binary image produced by an
   `inRange` filter on saturation/value: any pixel that is too dark or too unsaturated to be a marker is
   already discarded here. This is purely a coarse "could this *possibly* be a colored marker?" filter.
2. For each pixel that survived the mask, a **K-Nearest-Centers (KNC)** classifier runs on the GPU.
   It compares the pixel against every calibrated color and assigns it the most likely color ID, or
   `NONE` if no calibrated color is close enough.
3. The resulting per-pixel label image is passed through a **Moore-neighborhood filter**, which removes
   isolated pixels and keeps only labels that are corroborated by their surrounding neighbors.
4. The cleaned label map is returned. From here it leaves the identification stage and moves on to blob
   detection.

The whole journey is done on the GPU. Both the classification and the neighborhood filter are CUDA kernels
that run one thread per pixel, so a 1080p frame is processed in essentially constant time regardless of how
many colors are calibrated (up to a hard cap of 256).

## Algorithms

### Blob mask (pre-filter)

Before any classification happens, an `inRange` filter on the HSV image produces a binary mask using
user-configured thresholds for saturation and value. Pixels that fall outside the range are marked as
`NONE` directly and skipped by the classifier.

This is not really an "algorithm" in itself, but it matters: it is the only place where saturation and
value thresholds are applied globally. Doing it once up front means the classifier never wastes work on
obviously-empty pixels (background, shadows, the field surface itself).

### K-Nearest-Centers (KNC)

Each calibrated color is represented by a single point in HSV space - its **mean HSV value**, computed
during calibration. The classifier asks, for each pixel: *which calibrated color centers are closest to
me, and which color do they vote for?*

The distance used is a **weighted squared distance** in HSV:

```
distance = w_h * dh² + w_s * ds² + w_v * dv²
```

with weights `w_h = 4.5`, `w_s = 1.0`, `w_v = 1.5`. Hue is weighted most heavily because hue is what
actually identifies a color; saturation and value matter less and are mostly there to break ties. Hue is
also treated as **circular** (the difference between hue 5 and hue 175 is 10, not 170), since the hue
wheel wraps around at 180.

A pixel only participates if its closest center is within a maximum distance threshold (`max_dist = 50`).
If no center is close enough, the pixel is labeled `NONE`. This prevents the classifier from forcing
random pixels into a color just because they happen to be marginally closer to one center than another.

The "K" in KNC is the number of nearest centers each pixel keeps. The pipeline uses `k = 5`: the kernel
finds the 5 closest color centers within range and then takes a **majority vote** among them. With `k = 1`
the classifier collapses to a plain nearest-center, which is faster but more vulnerable to a single
mis-calibrated outlier center capturing pixels it shouldn't.

> Note on naming: the algorithm is called **K-Nearest-Centers** here, not K-Nearest-Neighbors. KNN
> classifies against every training *sample*; KNC classifies against one *centroid* per class. The
> mechanic - find the K closest, vote - is the same, but operating on one centroid per color is what
> makes this fast enough to run per-pixel on a live video stream.

### Moore neighborhood filter

After classification, the label map is still noisy: a single pixel here and there gets misclassified
because it sits on the border between two markers, or because of leftover sensor noise. A robot marker,
on the other hand, occupies a contiguous patch of dozens of pixels.

The Moore-neighborhood filter exploits this. For each labeled pixel, it looks at its 8 surrounding
neighbors (the **Moore neighborhood** - the eight cells around a center cell on a grid) and counts how
many share the same label. If at least `min_required = 6` of the 8 neighbors agree, the pixel keeps its
label. Otherwise it is reset to `NONE`.

The effect is twofold: isolated mis-classified pixels disappear entirely, and the contours of real
markers are slightly eroded, leaving a cleaner core. Border pixels of the image are also automatically
zeroed out, since they don't have a full neighborhood.

This step is intentionally simple - it is not a full connected-components pass, and it does not group
pixels into objects. That responsibility belongs to the next stage, blob detection. Here the only goal is
to denoise the label map so the blob detector has a clean input.

## Output

The output of identification is a single-channel image the same size as the input frame. Each pixel
holds either `NONE` (= 0) or one of the calibrated color IDs:

| ID | Color   |
|----|---------|
| 0  | NONE    |
| 1  | BLUE    |
| 2  | YELLOW  |
| 3  | CYAN    |
| 4  | GREEN   |
| 5  | MAGENTA |
| 6  | RED     |
| 7  | ORANGE  |

This label map is the only thing the rest of the system sees. From the blob detector's point of view,
the original camera image no longer exists - there are just regions of "blue" and "yellow" and "magenta"
on a black background, ready to be grouped into markers, then into robots.
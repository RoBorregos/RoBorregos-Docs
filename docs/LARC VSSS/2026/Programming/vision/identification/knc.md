# K-Nearest-Centroids (KNC)

KNC is the per-pixel classifier at the heart of the identification stage. It runs as a CUDA kernel
(`knc_dynamic_k_kernel`), one thread per pixel, and decides which calibrated color each pixel belongs to
— or that it belongs to none of them.

## The idea

Every calibrated color is represented in HSV space by one or more **centroids**. A centroid is just a single
HSV point: the mean HSV of a small region the user clicked on during calibration. A color like "blue" is
not represented by a single centroid but typically by several, one per click, all sharing the same
`Color_ID::BLUE` tag. This is important and shapes how the algorithm works.

For each pixel, KNC asks:

> *Which calibrated centroids am I closest to in HSV space, and which color do they vote for?*

The output is one `Color_ID` per pixel (or `NONE` if no centroid is close enough), written to a single-channel
label image the same size as the input frame.

## Why centroids and not samples

Plain K-Nearest-Neighbors compares each query against every training sample. That would be unworkable here:
calibration produces tens or hundreds of pixel samples per color, and the classifier has to run on every
pixel of every frame at video rate.

KNC sidesteps this by collapsing each calibration session into a single centroid (`hsv_avg`). The vector
uploaded to the GPU contains one entry per *click*, each click contributes one centroid, tagged with its
color ID, regardless of how many pixels were inside the click. A typical setup ends up with maybe 20–40
total centroids across all colors, which is small enough to scan linearly in a CUDA thread.

This is also why the algorithm is called **K-Nearest-centroids** rather than KNN: the data structure being
searched is centroids, not samples. The voting mechanic is the same; the search space is far smaller.

## The distance function

The kernel uses a **weighted squared distance** in HSV:

```
distance = w_h * dh² + w_s * ds² + w_v * dv²
```

with hardcoded weights:

```
w_h = 4.5   // hue
w_s = 1.0   // saturation
w_v = 1.5   // value
```

Hue dominates because hue is what actually identifies a color. Saturation and value matter, but mostly as
sanity checks: they prevent a pale or dim pixel from being matched to a vivid, bright centroid just because
its hue happens to align.

### Hue is circular

OpenCV's hue ranges from 0 to 180 and wraps around: hue 0 and hue 180 are the same color (red). A naive
absolute difference would say `|175 - 5| = 170`, when in reality those two hues are only 10 apart on the
color wheel. The kernel handles this with one line:

```cpp
float diff_h = fabsf(pixel.x - mean.x);
if (diff_h > 90.0f) diff_h = 180.0f - diff_h;
```

Anything beyond 90° is reflected back, giving the true circular distance. Without this, red pixels near
hue 179 would consistently fail to match a red centroid near hue 1, even though they're almost the same
color.

### Distance cap

Before a centroid is even considered for the vote, its distance must be below `max_distance` (passed in as
`max_dist² = 50² = 2500`). If no centroid clears this bar, the pixel is labeled `NONE`.

This is the *only* mechanism in the classifier that can produce a `NONE` output for a masked-in pixel.
Without it, every pixel would be forced into whichever calibrated color it was least dissimilar to, even
if that color was wildly different in HSV. The cap turns the classifier from "find the closest" into
"find the closest *if it's close enough*", which is what makes it usable in practice.

## The K-vote

The kernel keeps a running top-K list of closest centroids as it scans through them:

```cpp
float best_dist[MAX_K];   // sorted ascending
uchar best_ids[MAX_K];    // color IDs of the corresponding centroids
```

Whenever a new centroid beats one of the entries, the rest of the list shifts down and the new entry slots
in at the right position. This is just an insertion sort, but at K = 5 it's faster than calling a real
sort function. `MAX_K` is capped at 9 so the arrays can live in registers.

Once every centroid has been visited, the kernel has the K nearest centroids for this pixel, all guaranteed
to be within the distance cap. It then takes a **majority vote**:

```
for each unique color id in best_ids[]:
    count how many of the K slots vote for it
choose the id with the most votes
```

If `k = 1`, this collapses to plain nearest-centroid. With `k > 1`, the vote starts to matter, and it
matters specifically because of the multiple-centroids-per-color setup. Imagine a "blue" marker pixel
sitting between two blue centroids and one cyan centroid: nearest-centroid alone might pick cyan if it's
marginally closer, but a 5-vote across blue, blue, blue, cyan, magenta still resolves to blue. That's
the failure mode KNC is built to protect against.

In the current pipeline, `k` is hardcoded to 5 at the launch site (`launch_color_segmentation`).

## The mask short-circuit

The very first thing the kernel does after computing its pixel coordinates is consult the blob mask:

```cpp
if (mask(y, x) == Color_ID::NONE) {
   output(y, x) = Color_ID::NONE;
   return;
}
```

If the pixel was rejected by the mask, the kernel writes `NONE` and exits without touching any of the
distance math. This is what makes the upstream blob mask actually save work: the entire scan over
centroids, the K-list maintenance, and the vote are all skipped for background pixels.

## Calibration data on the GPU

The classifier reads its calibration from CUDA `__constant__` memory:

```cpp
__constant__ float3 c_means[MAX_COLORS];   // HSV centroids
__constant__ uchar  c_ids[MAX_COLORS];     // color ID for each centroid
__constant__ int    c_num_colors;          // how many entries are valid
```

`MAX_COLORS` is 256 (the cap on total centroids, not on distinct colors). Constant memory is broadcast-cached
on the GPU, so every thread in a warp reading the same centroid hits cache and sees no penalty. This is
exactly the access pattern KNC produces, every thread loops over the same `c_means` array, and is the
main reason the classifier scales well to high-resolution frames.

The data is uploaded once, in `upload_calibrations_to_gpu`, whenever calibration changes (e.g. when the
operator hits "save" in the blob calibration menu). It is *not* re-uploaded per frame.

## Output

For each pixel the kernel writes a single byte to the output image:

- `Color_ID::NONE` (0), the pixel was masked out, or no centroid was within the distance cap, or the K-vote
  resolved to no color.
- A calibrated color ID (1–7) otherwise.

This label image is the deliverable of identification. It still has noise (stray mis-classified pixels at
marker borders, isolated false positives), which is why the next stage runs the **Moore-neighborhood
filter** on it before handing the result off to blob detection.

## Tuning summary

The knobs that affect KNC behavior, and what they control:

| Parameter        | Where set                       | Effect |
|------------------|---------------------------------|--------|
| `max_dist`       | hardcoded in `launch_color_segmentation` (50) | Distance cap. Higher = more pixels classified, more risk of false positives. |
| `k`              | hardcoded in `launch_color_segmentation` (5)  | Vote size. Higher = more robust to outlier centroids, slightly slower. `k = 1` disables voting. |
| `w_h, w_s, w_v`  | hardcoded in the kernel (4.5, 1.0, 1.5)        | Channel weights in the distance. Heavier hue bias = stricter color discrimination. |
| `c_means, c_ids` | calibration save                | The actual centroids being searched. Quality and coverage of these dominate everything else. |

In practice the only one tuned regularly is the calibration itself, the rest are stable defaults that
were dialed in once. Bad classification almost always traces back to bad centroids, not to the algorithm.
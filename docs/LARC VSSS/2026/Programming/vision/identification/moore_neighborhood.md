# Moore neighborhood filter

The Moore neighborhood filter is the second and final kernel of the identification stage
(`label_moore_neighborhood_raw`). It runs immediately after the KNC classifier and operates on the same
label image, with one job: **clean up the noise**.

KNC works on each pixel in isolation — it has no idea what the surrounding pixels look like. That makes it
fast, but it also means a single noisy pixel sitting on the edge of two markers, or a stray pixel that
happened to land just inside the distance cap of some color, will be confidently mis-labeled. The Moore
filter throws those away by checking that each labeled pixel is *surrounded* by enough pixels of the same
label to be plausible.

## What is a Moore neighborhood

In a 2D grid, the **Moore neighborhood** of a cell is the 8 cells that surround it — the four orthogonal
neighbors (up, down, left, right) plus the four diagonals. It's the "king's move" neighborhood from chess.

```
  N N N
  N · N
  N N N
```

This is in contrast to the **von Neumann neighborhood**, which is just the 4 orthogonal neighbors.

The filter chose Moore over von Neumann because the diagonal neighbors carry real information at marker
borders: a real marker pixel at the corner of a marker may have most of its agreeing neighbors on the
diagonal, and a 4-neighborhood would miss them and erode the marker more aggressively than necessary.

## The algorithm

For each pixel `(x, y)` of the label image:

1. **Border check.** If the pixel is on the outer ring of the image (`x == 0`, `x == cols - 1`,
   `y == 0`, `y == rows - 1`), write `0` and stop. These pixels don't have a complete neighborhood, so
   the filter refuses to make a decision for them.
2. **Empty check.** Read the pixel's label. If it's `0` (`Color_ID::NONE`), write `0` and stop. There's
   nothing to clean up — the pixel was already classified as background.
3. **Count agreeing neighbors.** Walk the 8 surrounding pixels. For each one that carries the *same*
   label as the center pixel, increment a counter.
4. **Threshold.** If the count is at least `min_required`, the pixel keeps its label. Otherwise it's
   reset to `0`.

The threshold is passed in by the launch site as the last argument. The pipeline currently uses
`min_required = 6`, meaning at least 6 of the 8 neighbors must agree before a pixel is allowed to
survive.

## Effect of the threshold

The threshold is the only knob, and it has a very direct geometric interpretation:

| `min_required` | Behavior |
|----------------|----------|
| 1              | Pixel survives if even one neighbor agrees. Almost a no-op — only fully isolated pixels are removed. |
| 4              | Half the neighbors must agree. Removes thin lines and 1-pixel-wide protrusions. |
| 6              | Pixel must be *well inside* a region of its own color. Removes borders and isolated noise. **Default.** |
| 8              | Pixel must be *fully surrounded* by its own color. Aggressive erosion — only the deep interior of each region survives. |

At the default of 6, the filter has two related effects:

- **Noise removal.** Isolated mis-classified pixels and 1- or 2-pixel speckle disappear entirely. They
  can't possibly have 6 agreeing neighbors because they don't have 6 same-color neighbors to begin with.
- **Border erosion.** Pixels right at the boundary of a marker tend to have neighbors of mixed labels
  (some inside the marker, some outside it). The filter strips those off, leaving only the solid core of
  the marker behind.

That second effect is intentional and useful, even though it might look like a bug at first. Marker
borders are exactly where KNC is least reliable — a half-marker, half-background pixel could go either
way. By eroding them off, the filter hands a *conservative but trustworthy* core to blob detection,
which can compute centroids from the core just as well as it could from the full marker.

## What the filter does *not* do

It's worth being explicit about the boundaries of this step, because it sits next to operations that
look similar but are different in important ways:

- **It does not group pixels into objects.** No connected components are computed. The output is still
  a per-pixel label image, just a cleaner one. Grouping happens later, in blob detection.
- **It does not reassign labels.** A pixel can only keep its current label or become `NONE`. It can
  never *change color* — even if 7 of its 8 neighbors are blue and the pixel itself is yellow, the
  yellow label is checked against blue neighbors, finds 0 agreement, and the pixel is reset to `NONE`.
  It does not "flip" to blue.
- **It is not an opening or closing morphological operation.** Those operations work on binary images
  and use a single foreground class. This filter operates on a *multi-class* label image and the
  agreement check is per-color: a "blue" pixel surrounded by yellow neighbors looks just as isolated as
  a blue pixel surrounded by background, even though one is "in the middle of stuff" and the other
  isn't.

That last point is what makes this filter the right tool for the job here. A standard binary erosion
would leave a yellow-pixel-inside-a-blue-marker alone (it's surrounded by *something*), but the marker
is wrong and shouldn't be there. The Moore filter's per-color check correctly throws it out.

## Implementation notes

The kernel is a single CUDA kernel launched with the same 16×16 block configuration as KNC, one thread
per pixel. The work per thread is tiny: 8 reads from the input image, a counter, one write. There is no
shared-memory tiling — every thread reads its 9 pixels (itself plus 8 neighbors) directly from global
memory. At 1080p this still runs in well under a millisecond because the access pattern is highly
spatially coherent and the L1/L2 caches absorb the redundancy between neighboring threads.

A few specific properties of the implementation worth knowing:

- **Two separate buffers.** The kernel reads from `internal_buffer` (the KNC output) and writes to
  `output_buffer`. It does *not* operate in place. This is necessary because in-place writes would let
  one thread see another thread's already-cleaned output, breaking the assumption that "neighbor" means
  "neighbor in the original label image".
- **`__restrict__` pointers.** Both pointers are marked `__restrict__`, telling the compiler the input
  and output don't alias. This unlocks more aggressive load reordering and is part of why the kernel is
  cheap.
- **Border pixels are zeroed, not preserved.** The filter could have left the border with its KNC label,
  but instead it explicitly writes `0`. This is a deliberate simplification: the rest of the pipeline
  doesn't have to handle a special "edge of image" case, because no labeled pixel will ever exist on the
  border of the output.

## The pipeline

```
HSV frame ──► blob mask ──► KNC kernel ──► Moore filter ──► label map
                                                                │
                                                                ▼
                                                         blob detection
```

The Moore filter is the last thing identification does to the label map. After it returns, the GPU buffer
is downloaded to host memory (`label_map.download(...)` in `Detector::get_label_map`) and handed off to
the next stage as a regular `cv::Mat`. From this point on, no more per-pixel work happens — the rest of
the system thinks in terms of regions and centroids, not pixels.
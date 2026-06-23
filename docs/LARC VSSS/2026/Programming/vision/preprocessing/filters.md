# Applying filters

Filters in preprocessing are the key to a successful identification. Without filters, the pixels color may vary
significantly in a way that noise or even the smallest change in illumination can completely ruin an identification.

## Frame journey
The frame journey looks like this, filters will be explained at a deeper level in the following sections

1. Frame is received
2. Color boosts are applied
3. The frame color format is converted from BGR to Lab (see explanation in the following sections)
4. Lab channels are split
5. Contrast Limited AHE is applied to the L channel
6. Channels are merged
7. Bilateral filter is applied
8. The image is converted from Lab to HSV
9. The channels are split
10. In the saturation channel, a multiplicator is applied (called saturation)
11. A Gamma filter for Saturation and Value is applied
12. The image is merged
13. In some cases, a manual exposure boost may be needed (see following sections)
14. Image is converted from HSV to BGR

## Color formats

A color format (or *color space*) is a specific way of representing the color of a pixel as a tuple of numbers. The
same physical color can be encoded in many different formats, and each one arranges the information along different
axes. Choosing the right format for each step matters, because some operations are trivial in one space and almost
impossible in another.

The pipeline works with three color formats:

- **BGR** is the native format used by OpenCV and most cameras. Each pixel is described by three numbers
  representing the intensity of the Blue, Green and Red channels. It is convenient for input/output, but a poor
  representation for image processing: brightness and color are tangled together across all three channels, so
  changing one channel changes both at the same time.
- **Lab** splits the image into a perceptual lightness channel (L) and two chromaticity channels (a, b) that
  describe color independently of how bright the pixel is. This decoupling is what makes it ideal for contrast
  enhancement.
- **HSV** describes pixels in terms of Hue (the actual color tone), Saturation (how vivid the color is) and Value
  (how bright it is). This is the format the rest of the system uses for color-based segmentation, since the
  identification thresholds are defined in HSV.

### ¿Why Lab instead of BGR?

Contrast enhancement and edge-preserving smoothing both want to operate on *brightness*, not on color. In BGR there
is no such thing as a brightness channel: brightness is encoded jointly across B, G and R, so applying CLAHE
independently to each BGR channel would shift the color of every pixel and destroy the chromatic information the
detector relies on later.

Lab solves this by isolating brightness in the L channel, while a and b carry only chromatic information. The
pipeline can then apply CLAHE exclusively to L (improving local contrast) and bilateral filtering on the full Lab
image (smoothing noise without bleeding colors across edges). After both filters, the colors of the markers are
preserved almost intact, just on top of a much cleaner luminance.

A second reason is that Lab is approximately *perceptually uniform*: equal numerical distances correspond to roughly
equal perceived color differences. This makes it a much better space for filters that operate on neighborhoods of
pixels, like the bilateral filter.

### ¿Why HSV instead of Lab?

Lab is great for cleaning the image, but it is awkward for *segmenting by color*. The a and b channels mix hue and
saturation together, so isolating "any shade of yellow" or "any shade of blue" requires defining a complicated 2D
region in the a-b plane.

HSV is the natural format for this task. Hue alone tells which color tone a pixel has, regardless of how bright or
washed out it is, so the calibration can specify a marker as a simple range like `H ∈ [105, 110]`. Saturation and
Value then act as confidence axes: pixels with very low saturation are gray and should be rejected, and pixels with
extreme values are over- or underexposed and unreliable. HSV also makes the last preprocessing steps (saturation
multiplier and gamma correction on S/V) trivial: each one acts on a single, meaningful channel.

In short: Lab is used to *clean* the image, HSV is used to *describe* it.

## Filters

Each filter targets a specific failure mode of raw camera frames: bad white balance, low local contrast, sensor
noise, washed-out colors, or non-linear brightness response. They are applied in a fixed order because the output
of each filter is the assumed input of the next one.

### Color boosts

Color boosts are simple per-channel multipliers applied directly on the BGR image, with one independent gain for
Blue, Green and Red (`blue_boost`, `green_boost`, `red_boost`). When all three are equal to `1.0`, the boost step
is skipped entirely and the source image is just copied through.

Their purpose is to compensate for the white balance of the camera and the color temperature of the field
lighting. Most webcams shipped with Very Small Size Soccer setups have a noticeable bias (often a green or blue
tint), and the field lights add another bias on top. Without correcting it, a "yellow" marker might end up sitting
on the boundary between yellow and green in HSV, making segmentation fragile.

By boosting each channel independently before any other filter runs, the pipeline normalizes the raw image so that
white surfaces look approximately white and saturated colors land where the calibration expects them. This is done
first because every later filter assumes a reasonably balanced input.

### Contrast Limited AHE (CLAHE)

CLAHE stands for *Contrast Limited Adaptive Histogram Equalization*. Standard histogram equalization stretches the
brightness histogram of an entire image so that dark and bright regions use the full available range. The problem
is that it is global: a single bright reflection can dominate the histogram and crush the contrast of the rest of
the frame.

Adaptive histogram equalization fixes this by dividing the image into tiles (the pipeline uses a 32×32 grid) and
equalizing each tile independently, so dark and bright regions are enhanced according to their own local
distribution. The "Contrast Limited" part adds a clip on how aggressively each tile can be equalized, which
prevents the algorithm from amplifying noise in nearly-flat regions. The strength of this clip is controlled by
the `clahe_clip_limit` parameter.

Crucially, CLAHE is applied **only to the L channel of the Lab image**, never to a or b. This is the whole reason
the pipeline switches into Lab beforehand: applying CLAHE to a brightness-only channel improves local contrast
without changing any pixel's hue.

### Bilateral

The bilateral filter is an edge-preserving smoothing filter. A normal Gaussian blur averages each pixel with its
neighbors based purely on spatial distance, which destroys edges along with the noise. The bilateral filter adds a
second weighting term based on color similarity: neighbors that are similar in color contribute to the average,
neighbors on the other side of an edge do not.

The result is an image where flat regions get cleaned up but the contours of the markers stay sharp. Edge sharpness
is what later allows the blob detector to lock onto stable centroids.

The pipeline runs the bilateral filter on the merged Lab image after CLAHE, so that the smoothing acts in a
perceptually uniform space. The strength of the filter is controlled by `bilateral_sigma`, which sets both the color
and (implicitly) the spatial weighting. Setting `bilateral_sigma` to `0` effectively disables the filter.

### Gamma Saturation/Value

Cameras and displays do not respond to light linearly: a pixel value of `128` is not "half as bright" as a pixel
value of `255`. Gamma correction is a non-linear remapping of channel values via the curve
```cpp
out = (in / 255) ^ (1 / gamma) * 255
```

When `gamma > 1`, dark values get pushed up faster than bright ones, brightening the image without saturating the
highlights. When `gamma < 1`, the opposite happens, useful when bright regions are overexposed. When `gamma == 1`
the curve is the identity and the step is skipped.

The pipeline applies gamma correction independently to the S and V channels of the HSV image
(`gamma_correction_s` and `gamma_correction_v`). Correcting V reshapes how brightness is distributed: a slightly
underexposed frame can be lifted into the range the calibration was done at, without clipping. Correcting S
reshapes how vividness is distributed: it lets the pipeline pull mid-saturation pixels (which are the ambiguous
ones during segmentation) toward either gray or full color, depending on the curve.

For performance, the gamma curve is precomputed into a 256-entry **Look-Up Table (LUT)** on the GPU and only
recomputed when the gamma value actually changes. Applying the filter is then just a single LUT lookup per pixel.

### Saturation

The saturation filter is the simplest of all: a plain multiplier applied to the S channel of the HSV image,
controlled by the `saturation` parameter. A value of `1.0` is a no-op; values greater than `1.0` make all colors
more vivid, values smaller than `1.0` desaturate the image toward gray.

It is applied **before** the gamma correction on S, so the order of operations on the saturation channel is:
multiply, then re-shape with gamma. This matters because gamma is non-linear: applying gamma first and then
multiplying would give a different result, and the calibration is done assuming this specific order.

In practice, saturation is the main "knob" used to recover faded colors when the field lights are weak or when the
camera's auto white-balance has washed the image out. Gamma on S is then used for the finer tuning.

## Manual exposure

Manual exposure is an optional, conditionally-enabled step that lives in the `manual_exposure` function. It is
**not** part of the default frame journey: the call site is left commented out in the pipeline and is intended to
be enabled only when needed.

The reason it exists is that the camera's auto-exposure can fight against the rest of the pipeline. When the
field's average brightness changes rapidly (a person walks past a light, the field is partially shaded, the camera
re-meters in the middle of a play), the auto-exposure pushes the global brightness up and down, and every gain that
was carefully calibrated against a reference image is suddenly wrong.

Manual exposure compensates for this by computing the **mean** of the S and V channels of the current frame and
multiplying each channel by whatever factor brings that mean to a fixed target (`30` for saturation, `43` for
value). The effect is to renormalize every frame to the same average vividness and brightness, regardless of what
the camera's auto-exposure decided. After this step, the per-color HSV thresholds defined during calibration apply
again.

The reason it is disabled by default is that it can hurt as often as it helps: when the camera is well-behaved and
the lighting is stable, forcing the mean to a target value just adds noise to the channels and reduces the dynamic
range of the colors. The rule of thumb in the code comment applies: **uncomment it only if exposure changes rapidly
within a short period of time**.
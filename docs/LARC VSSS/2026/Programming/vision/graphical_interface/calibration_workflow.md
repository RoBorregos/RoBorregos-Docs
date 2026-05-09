# Calibration workflow

This document describes the **operator-facing procedure** for calibrating the vision system before a
match. Each individual calibration tool is documented separately (in the calibration section of the
docs); this is about the *order* in which they need to be run, *why* that order matters, and what each
step depends on from the previous ones.

If you're new to the system, this is the page you want.

## TL;DR

```
1. Camera         — physical setup; brightness, exposure, focus
2. ROI            — define the field polygon (4 corners)
3. Mask           — saturation/value floor (cut field, shadows, dark robots)
4. Color filters  — preprocessing knobs (saturation, gamma, CLAHE, boosts)
5. Blob calib     — sample colored markers, one color at a time
6. Detection      — verify everything works end-to-end
```

Each step's output is consumed by the next. Skipping ahead, or revisiting an earlier step without
redoing the later ones, will produce inconsistent results.

## Why the order matters

The pipeline is **layered**, and each calibration tunes a different layer:

```
camera       → produces the raw frame
   │
   ▼
ROI          → blacks out everything outside the field
   │
   ▼
mask         → coarse "is this a colored marker?" filter
   │
   ▼
preprocessing → cleans the colors via filters
   │
   ▼
blob calib   → tells the classifier what each color looks like
   │
   ▼
detection   → reads the cleaned colors and outputs robots
```

If the camera is mis-exposed, every downstream step is calibrated against bad data. If the ROI isn't
defined, the mask has to deal with the entire frame including walls and floor. If the preprocessing
filters change after blob calibration, the colors the classifier was taught no longer match the colors
it sees. The order isn't arbitrary, it's the order in which information *flows* through the pipeline.

The rule of thumb: **calibrate upstream first, downstream last.** Any time an upstream parameter is
changed, every downstream calibration that depended on it has to be redone.

## Step-by-step procedure

The calibration tools live in the main menu (`MODO: ...` buttons). Each one switches the application
into a state that shows a different sidebar with the relevant sliders, buttons, or click targets.

### 1. Camera calibration

**Mode:** `MODO: CAMERA CALIBRATION`

**What it does:** writes camera parameters directly to the V4L2 device via `cv::VideoCapture::set`:
brightness, contrast, auto-exposure, exposure, auto-focus, focus.

**What you're aiming for:** an image that looks roughly correct *to the human eye*. Markers should be
visible and distinct, the field surface shouldn't be blown out or crushed to black, and the image
should be in focus across the whole field. Don't try to fix color casts here, that's what the color
filters are for.

**Important:** the camera calibrator pushes changes to the device *immediately* (via the slider
`on_change_callback → upload_to_camera`). So as you drag sliders, the camera responds in real time. Save
when you're satisfied.

**Auto vs. manual:**
- `auto_exposure` should normally be **off**. Auto-exposure adjusts on the fly as the field's average
  brightness changes (a person walks past a light, a robot moves through a shadow), which means every
  frame's calibration is slightly different. Fixed exposure is more reproducible.
- `auto_focus` should be **off**. The camera-to-field distance is fixed; once focus is set there's no
  reason to keep refocusing.

**Why this is first:** every other step looks at the raw frame. If the camera is mis-configured, you'll
calibrate the rest of the pipeline against a moving target.

**Save and exit:** "GUARDAR Y VOLVER" writes the parameters to the calibration JSON and returns to the
main menu. The values also persist on the camera itself until power cycle.

### 2. ROI (Region of Interest)

**Mode:** `MODO: ROI CALIBRATION`

**What it does:** defines a polygon on the field, typically the four corners of the playing area,
that gets used for **two** distinct purposes:

1. **Mask cutoff.** Pixels outside the polygon are blacked out *before* the GPU preprocessing runs (via
   the inverse-polyline draw command on the `PREPROCESSING` layer). The classifier never sees the
   audience, the table edges, the cables, they're zeroed out at the pixel level. This drastically
   reduces false detections from background clutter.
2. **Homography.** The four corners of the ROI define the mapping from image coordinates to real-world
   field coordinates. When you click "GUARDAR Y VOLVER", `coordinates->update_matrix()` is called and
   the homography matrix is recomputed from the new corners.

**How to use it:**
- Left-click on each corner of the field, in order.
- "Remove Last Point" undoes the most recent click.
- "Reset Points" clears the polygon.
- "GUARDAR Y VOLVER" saves and recomputes the homography.

**Important:** the ROI polygon should be **exactly 4 points** for the homography to work. The mask
cutoff itself accepts any polygon with > 1 points, but tracking will fail if the homography can't be
computed.

**Click ordering matters for the homography**, but the code's `get_sorted_corners` re-sorts internally,
so the operator can click in any order, top-left, top-right, bottom-right, bottom-left, or any
permutation.

**Why this is second:** the mask threshold step (next) is much easier when only the field is visible.
A mask that has to ignore audience clothing and overhead lights will be either too aggressive (cutting
real markers) or too lax (letting in background colors).

**Save and exit:** "GUARDAR Y VOLVER" writes the polygon to the calibration JSON, recomputes the
homography, and returns to the main menu.

### 3. Mask thresholds

**Mode:** `MODO: MASK CALIBRATION`

**What it does:** sets the six thresholds (`h_min`, `s_min`, `v_min`, `h_max`, `s_max`, `v_max`) used
by the blob mask `inRange` filter. Pixels outside this range are skipped by the KNC classifier.

See the [blob mask documentation](./blob_mask.md) for the underlying algorithm.

**What you're aiming for:** all colored markers stay visible (white) in the mask preview, and
everything else (field surface, robot bodies, shadows) gets cut to black.

**The pattern that works in practice:**
- **Hue (`h_min`, `h_max`):** leave at the full range `[0, 180]`. Hue is what the *classifier* uses to
  tell colors apart, so cutting hue here would reject entire colors before they reach KNC.
- **Saturation (`s_min`):** raise until the field surface (which is desaturated grey/white) disappears
  but the markers stay. Typical value: 15–30.
- **Value (`v_min`):** raise until shadows and dark robot bodies disappear. Typical value: 0–90,
  depending on lighting.
- **`s_max`, `v_max`:** leave at 255 unless you have a specific reason to clip the high end (rare).

**Important:** the mask has to be **looser than you think**. If a real marker pixel is rejected by the
mask, the classifier never sees it, the marker comes out smaller than it should, or disappears
entirely. Err on the side of letting too much through; the classifier will reject background pixels via
its distance cap.

**Why this is third:** the preprocessing color filters (next step) modify saturation and value. Setting
the mask now gives a baseline to work against; if you set it after the filters, any later tweaking of
saturation or gamma_v will require redoing the mask.

**Save and exit:** "GUARDAR Y VOLVER" persists the mask parameters and returns to the main menu.

### 4. Color filters (preprocessing)

**Mode:** `MODO: COLOR CALIBRATION`

**What it does:** sets the parameters used by the GPU preprocessing pipeline, saturation multiplier,
gamma on S, gamma on V, CLAHE clip limit, bilateral sigma, and per-channel BGR boosts.

See the [preprocessing documentation](./preprocessing/index.md) for the underlying filters.

**What you're aiming for:** colors that match what KNC will be calibrated against in the next step.
Markers should be **vivid and clearly distinct** from each other after this step. The filters do most
of the work in turning a meh-looking raw frame into a saturated, high-contrast image where each color
has a tight HSV signature.

**Suggested order within this step:**
1. **Color boosts** (`Red Boost`, `Green Boost`, `Blue Boost`), fix any white-balance bias. Aim for
   white surfaces to look white. These default to 1.0 (no change).
2. **CLAHE clip limit**, raise to bring out local contrast, especially in shadowed parts of the field.
3. **Saturation**, raise until colors look vivid but not blown out. Typical: 1.0–1.5.
4. **Gamma S, Gamma V**, fine-tune. Gamma > 1 brightens, gamma < 1 darkens. Useful for shifting the
   "average" color into the calibration zone.
5. **Bilateral sigma**, only if you see noisy speckle in the markers. Set to 0 to disable.

**Important:** *do not* set these to wildly different values from one match to the next. The blob
calibration in step 5 is recorded **after** these filters run, so the calibrated HSV values are tied to
this filter configuration. If you change the filters later, the colors the classifier was taught no
longer match what it sees.

**Why this is fourth:** the blob calibrator (next) records the HSV mean of pixels *as they appear after
preprocessing*. So the filters have to be locked in before any blob samples are taken. If you change a
filter after a blob calibration, redo the blob calibration.

**Save and exit:** "Reset Filter" reverts to defaults; "GUARDAR Y VOLVER" persists.

### 5. Blob calibration

**Mode:** `MODO: BLOB CALIBRATION`

**What it does:** for each of the 7 colors (BLUE, YELLOW, RED, GREEN, CYAN, MAGENTA, ORANGE), record
one or more sample regions and compute their HSV statistics. The mean HSV becomes a *center* in the
KNC classifier's color space. See the [KNC documentation](./identification/knc.md) for what the
classifier does with these centers.

**The flow per color:**
1. From the blob calibration menu, click the color button (e.g. "BLUE"). This enters the
   `BLOB_CALIBRATING` state and clears any in-progress sample points.
2. **Click 4 points** on the field, around a visible marker of that color, forming a quadrilateral that
   tightly encloses the colored region.
3. The calibrator runs a CUDA kernel (`launchBlobKernel`) over the masked-in pixels inside the
   quadrilateral, computes the mean / stddev / min / max of HSV across those pixels, and stores the
   result as a `CalibrationResult`.
4. Click "GUARDAR Y VOLVER" to commit the result. The button label updates to e.g. "BLUE - OK (1)" to
   show how many samples have been recorded for this color.
5. Return to the color picker and repeat for every other color.

**Multiple samples per color:** every "GUARDAR" appends a new sample to the list. More samples = more
KNC centers for that color = more robust matching across lighting variation. Three or four samples per
color, taken from markers in different parts of the field, typically gives good coverage.

**The 4-point requirement:** `handle_click` accepts up to 4 points. The blob kernel only runs if
exactly 4 are present (`if (points.size() != 4) return std::nullopt`). Fewer than 4 = no result; more
than 4 are silently rejected.

**Edge case, hue wrapping (red):** red sits at the boundary of the hue wheel (around 0/180), so a red
patch can have pixels with hue values both near 0 and near 180. The blob kernel detects this case via
`count_low_red` and `count_high_red`; if both are nonzero, it switches to **wrap-around statistics**
(`sum_h_wrap`, `min_h_wrap`, etc.) that treat the hue ring as continuous. The final `avg_h` is then
adjusted via `if (avg_h >= 180.0) avg_h -= 180.0` to stay in the valid range. This is invisible to the
operator, it just means red calibrates correctly even when the samples straddle the hue wrap.

**Edge case, tiny ROI:** the kernel masks out the 5-pixel border of the quadrilateral via an erosion
(to skip color leakage at the marker edges). If the quadrilateral is small enough that erosion empties
the mask entirely, the calibrator falls back to the unmodified mask. So very small calibration regions
still work, just without the edge-protection.

**Important:** if the calibration UI says "[NO CALIBRADO]" for a color when you click "IMPRIMIR
CALIBRACIONES", that color has no samples. The classifier will literally never produce that color in
its output. Calibrate every color you expect to see on the field.

**Why this is fifth:** every previous step affects what the camera-to-frame pipeline produces.
Recording the colors *after* all of them are locked in is the only way the recorded values match what
the classifier will see at runtime.

**Save and exit:** each color's "GUARDAR Y VOLVER" calls `save_current_blob_calibration`, which appends
to `match_calibration` and immediately uploads the new calibration set to the GPU via
`detector->upload_calibrations`. The classifier picks up the change on the next frame.

### 6. Detection

**Mode:** `MODO: DETECTION`

**What it does:** runs the full pipeline end to end, displaying detected robots and the ball as
overlays on the live frame.

This is the verification step. If anything is off, a robot's ID is unstable, the ball flickers between
red and orange, a robot is invisible, the cause is in one of the earlier steps. The recovery is to go
back and redo that step. Common diagnoses:

| Symptom                                | Likely cause                                  | Where to fix |
|----------------------------------------|-----------------------------------------------|--------------|
| Random pixels labeled as colors        | Mask too lax (low `s_min`/`v_min`)            | Mask         |
| Markers come out smaller than expected | Mask too aggressive                           | Mask         |
| Color flickers between two IDs         | Two color centers too close in HSV; or insufficient blob samples | Blob calib   |
| Whole color category never detected    | That color has no blob samples                | Blob calib   |
| Colors look washed out / off           | Color filters not aggressive enough           | Color filters|
| Robot positions don't match the field  | ROI corners wrong / clicked in wrong order   | ROI          |
| Ball flickers with the field/red robots| Orange/red ambiguity; lighting not vivid enough | Color filters|
| Detection breaks under shadow          | Auto-exposure on, or too-aggressive `v_min`   | Camera / Mask |
| Everything works in one half of field  | Field illumination uneven                    | Camera / CLAHE|

## Persistence and reload

Every save step writes to a single calibration JSON file (`vsss_calibration.json` by default,
configurable). The file holds:

```json
{
  "color_calibration": { /* preprocessing filter parameters */ },
  "camera_calibration": { /* brightness, exposure, focus */ },
  "mask_params":        { /* h_min, s_min, ..., v_max */ },
  "roi_points":         [ /* polygon corners */ ],
  "blue":   [ /* one entry per BLUE blob sample */ ],
  "yellow": [ /* ... */ ],
  ...
}
```

On startup, every calibrator's `load_calibration` reads this file and populates the corresponding
`AppData` field. If the file is missing or empty, defaults are used; if the JSON is corrupt, the file
is renamed to `<filename>.corrupt` and a fresh one is generated.

This means **calibration is durable across runs**. The first match of the day requires the full
procedure; subsequent runs only need to verify (step 6) and re-tune anything that drifted (typically
nothing).

## When to redo what

Calibration drifts. Conditions in the venue change. Use this rough guide:

| Trigger                                          | Redo from step |
|--------------------------------------------------|----------------|
| New camera, new lens, or camera moved            | 1 (everything) |
| Field moved, or table rotated                    | 2 (ROI onward) |
| Lighting changed significantly (different room, time of day, lights replaced) | 3 (mask onward) |
| Markers replaced (different paint, fading)       | 5 (blob calib only) |
| Just want to clean up flaky color separation     | 5              |

The "redo from" column is conservative, sometimes you can get away with less, but each downstream step
*assumes* the upstream ones haven't changed. When in doubt, redo more rather than less.

## A note on real-time tuning

All sliders in the calibration UI bind directly to the parameter struct that the next frame's
preprocessing reads (see [the GUI doc](./gui/index.md)). This means you can watch the effect of every
adjustment in real time, on the live camera feed. Take advantage of it, drag sliders while watching
the detection output, not while staring at numbers.

The pause button (in the main menu) freezes the frame so you can examine a specific moment without
the field moving. Useful when calibrating against a particular configuration (e.g., a robot under a
specific shadow).
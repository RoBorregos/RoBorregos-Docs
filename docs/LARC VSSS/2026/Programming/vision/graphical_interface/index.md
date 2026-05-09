# Graphical interface

The graphical interface is the operator-facing layer of the vision system. It is *not* the user-facing
output of the pipeline (that's ROS messages, consumed by other nodes), but rather the **debug, tuning,
and calibration console** that the operator sees during a match or training session: the live frame, the
overlays drawn on top of it, the menu of modes, and the sliders/buttons that change parameters in real
time.

It's built directly on top of OpenCV's `cv::imshow` window and mouse callbacks, no external GUI
toolkit. The choice keeps the dependency footprint small and keeps the interface running anywhere the
detector itself runs (notably, inside the same OpenGL/CUDA context that the GPU pipeline uses).

## What the interface shows

The window is split into two regions, side by side:

```
┌──────────────────────────────────┬─────────────────┐
│                                  │  rowspace 0     │
│                                  ├─────────────────┤
│                                  │  rowspace 1     │
│         camera image             ├─────────────────┤
│      (with overlays drawn        │  rowspace 2     │
│         on top)                  ├─────────────────┤
│                                  │       ...       │
│                                  ├─────────────────┤
│                                  │  rowspace 9     │
└──────────────────────────────────┴─────────────────┘
```

The **left two-thirds** is the live frame: whatever the camera (or simulator, or recorded video) is
producing, after preprocessing, with detection overlays drawn on top, robot patches, centroids, facing
arrows, ball position, ROI outline, calibration markers, and so on.

The **right one-third** is the **sidebar**: a vertical strip of 10 fixed slots (called *rowspaces*) that
holds buttons, sliders, toggles, and text labels. Each slot is the same height. Different application
states show different widgets in these slots, but the layout stays consistent.

A small **FPS counter** with a spinning indicator is drawn in the top-left of the window every frame, so
the operator can see at a glance whether the pipeline is keeping up with the camera.

## Architecture

The interface is built from three cooperating components, each in `gui/`:

```
GUI                 , the canvas. Owns the window, the frame buffer, viewport,
                       and the rowspace coordinate system. Provides primitives:
                       text, plot, circle, line, polyline.

Drawer              , a deferred draw-command queue. Layers (PREPROCESSING,
                       MARKINGS, INTERFACE) collect commands and flush them
                       at the right moment in the pipeline.

InterfaceManager    , the controller. Owns the widgets for each application
                       state, dispatches mouse events to the active widget set,
                       and routes state transitions.
```

Plus the widget hierarchy: `Widget` → `Button`, `Slider`, `Toggle`. Each widget binds to a piece of
application state (a parameter pointer, a callback) and renders itself into one rowspace slot.

## The Drawer: layered, deferred drawing

Most of the rendering is **not immediate**. When the detector calls `gui->solid_circle(...)` to mark a
robot's centroid, no drawing happens yet, the circle is recorded as a `CircleCmd` and pushed into a
queue tagged with a layer:

```cpp
queues[layer].push_back(std::make_unique<CircleCmd>(center, radius, thickness, color));
```

There are three layers, flushed in a fixed order during the frame:

1. **`PREPROCESSING`**, drawn *before* the GPU preprocessing runs. This layer is for marks that need
   to be baked into the pixels before filters touch them, like the inverse-polyline that blacks out the
   area outside the ROI. By the time the frame reaches the classifier, those pixels are already zero
   and won't produce false detections.
2. **`MARKINGS`**, drawn on the camera image *after* it's downloaded back from the GPU but before it's
   shown. This is where detection overlays live: robot circles, facing arrows, ball markers, anything
   that should appear "on top of" the live image.
3. **`INTERFACE`**, drawn last, on the assembled display buffer (image + sidebar). This layer covers
   the widgets, text labels, and any UI chrome that lives independently of the image.

The render order matters because the layers are flushed at *different points* in `GUI::display_frame`
and `GUI::upload_frame`, not just by z-order on the same canvas. The Drawer just provides the queue
and a `render(target, layer)` method that drains the queue for one layer into the given image buffer.

This pattern keeps the rendering decoupled from the data flow. The detector can call `gui->plot(...)`
from anywhere, even from inside a CUDA download path, without worrying about when the actual paint
happens. The Drawer guarantees the command lands on the right layer and is flushed at the right time.

The command pattern is the standard polymorphic `DrawCommand` base class with one subclass per
primitive: `PlotCmd`, `LineCmd`, `CircleCmd`, `PolylineCmd`, `InversePolyCmd`, `TextCmd`. Each implements
a single `draw(target)` method that runs the underlying OpenCV call.

## The GUI: canvas and coordinate systems

The `GUI` class is the canvas. It owns:

- The OpenCV window (`cv::imshow` with a stable name).
- The current frame on GPU (`image_bgr`, `image_hsv`).
- The composite display buffer (`image_with_gui`) that combines the image and the sidebar.
- The viewport, a `cv::Rect` describing what region of the frame is currently visible (this is what
  zoom modifies).

There are two coordinate systems the operator interacts with, and the GUI has to translate between
them on every click:

- **Screen coordinates**, pixel positions inside the displayed window. This is what the mouse
  callback gives us.
- **World coordinates**, pixel positions inside the original (unzoomed) frame. This is what the
  classifier and calibration code want.

`GUI::screen_to_world` does the conversion. When the viewport is the full image, the two are equal.
When the user has zoomed in, screen and world differ, and a click at screen `(150, 200)` might
correspond to world `(870, 412)`. Every input handler that wants to know "what pixel did the user
click on" calls `screen_to_world` first.

Clicks in the sidebar (where `screen_x >= image_width`) are passed through unchanged, those are widget
clicks, not image clicks.

### The rowspace system

The sidebar is divided into a fixed grid of **10 rowspaces**, computed by `GUI::rowspace_roi`:

```cpp
const int x = image_width + margin;
const int y = (total_height / 10) * rowspace + margin;
const int width = (total_width - image_width) - 2 * margin;
const int height = (total_height / 10) - 2 * margin;
```

Each rowspace is a `cv::Rect` derived from its index (0 to 9). Widgets are constructed with their
target rowspace as a parameter, and they call back into `gui->rowspace_roi(rowspace)` whenever they
need to draw or hit-test. This means the widgets don't track their own positions, the layout is
purely a function of the rowspace index and the current window size.

The benefit is **layout-by-position-only**: no nested containers, no padding rules, no auto-layout.
A widget at rowspace 3 is at slot 3, every time. Different application states reuse the same 10 slots
for different sets of widgets.

The cost is that every menu has to fit inside 10 slots, and there's no way to scroll. So far that's
been enough.

### Zoom

Zoom is mouse-wheel-driven (right-button-down in the current input handler) and operates on the
viewport. Scrolling shrinks or grows the rectangle by 10% per step, anchored at the mouse position so
the pixel under the cursor stays under the cursor. Middle-click resets the zoom to the full image.

The zoom is *purely a display effect*: only the visible region of the frame is resampled to the
display image's dimensions in `display_frame`. The underlying image, the detector, and the calibrator
all keep working in original pixel coordinates regardless of the current zoom level. Zoom is a
convenience for the operator (especially for blob calibration, where clicking on a single pixel
matters), nothing more.

## The InterfaceManager: state and dispatch

The interface is a state machine. The current state lives in `app_data->current_state` (an `AppState`
enum) and the `InterfaceManager` switches the active widget set based on it:

| State                       | What it shows                                                  |
|-----------------------------|----------------------------------------------------------------|
| `MAIN_MENU`                 | Mode selection: detection, blob calib, color calib, etc.       |
| `DETECTION`                 | Live detection overlay; minimal sidebar (just "VOLVER").       |
| `BLOB_CALIBRATION_MENU`     | Color picker (BLUE, YELLOW, RED, GREEN, CYAN, MAGENTA, ORANGE).|
| `BLOB_CALIBRATING`          | Active calibration session for the picked color: reset / save. |
| `COLOR_CALIBRATING`         | Sliders for `VisionParams` (saturation, gamma, CLAHE, etc.).   |
| `CAMERA_CALIBRATING`        | Sliders + toggles for camera (brightness, focus, exposure).    |
| `MASK_CALIBRATING`          | 6 sliders for blob mask thresholds.                            |
| `ROI_CALIBRATING`           | Click-to-add ROI polygon points; remove last / reset / save.   |

Each state has its own `std::vector<std::unique_ptr<Widget>>` populated once at startup in
`init_widgets()`. The `draw_interface()` method picks the right vector based on `current_state` and
calls `widget->draw()` for each; `handle_input(event, x, y)` does the same with `widget->handle_input`.

Widgets *consume* events: if a widget returns `true` from `handle_input`, the click is considered
handled and won't fall through to the image-area handler. That's how the calibrators (which interpret
clicks on the image as calibration points) coexist with the sidebar widgets, the sidebar gets first
crack, and only unconsumed clicks reach the calibrators:

```cpp
if (!widget_consumed && app_data->current_state == AppState::BLOB_CALIBRATING) {
    BlobCalibrator::on_mouse(event, x, y, 0, blob_calibrator);
}
if (!widget_consumed && app_data->current_state == AppState::ROI_CALIBRATING) {
    ColorCalibrator::on_mouse(event, x, y, 0, color_calibrator);
}
```

There's also a **double-click probe** that works in any state: double-clicking on the image prints the
HSV value of that pixel to the console. It's a quick way to sanity-check the preprocessing without
opening a calibration mode.

## Widgets

The widget hierarchy is small and deliberately so. Each widget is one rowspace, one piece of state, one
callback.

### `Widget` (base class)

Holds a pointer to the GUI, a rowspace index, a label, an `is_hovered` flag, and an `on_change`
callback. The base `handle_input` only updates the hover state; subclasses override it to actually do
something.

```cpp
bool Widget::handle_input(int mouse_x, int mouse_y, int event) {
    cv::Rect roi = gui->rowspace_roi(rowspace);
    is_hovered = roi.contains({mouse_x, mouse_y});
    return false;
}
```

Hover detection is just rectangle containment. The `is_hovered` flag is then used by subclasses to
change appearance (lighter background on a hovered button, brighter knob on a hovered slider).

### `Button`

A clickable rectangle with a text label. Clicking inside the rectangle fires the callback:

```cpp
if (is_hovered && event == cv::EVENT_LBUTTONDOWN) {
    if (on_change_callback) on_change_callback();
    return true;
}
```

Returns `true` to consume the event. Most state transitions in the system (entering/leaving calibration
modes, saving, resetting, exiting) are buttons.

### `Slider`

Binds to a `float*` and a `[min, max]` range. Renders as a horizontal bar with a circular knob. While
the user is dragging the knob, mouse position is mapped linearly to the value, clamped to the range,
and the optional callback fires on each change.

```cpp
float ratio = static_cast<float>(mouse_x - start_x) / static_cast<float>(bar_width);
ratio = std::clamp(ratio, 0.0f, 1.0f);
*value_ptr = min_v + ratio * (max_v - min_v);
if (on_change_callback) on_change_callback();
```

Sliders bind directly to the parameter struct's fields:

```cpp
std::make_unique<Slider>(gui, 1, "Gamma S", &app_data->color_params.gamma_correction_s, 0.0f, 5.0f);
```

So moving a slider updates the live parameter, which is read by the next frame's preprocessing pass.
Tuning happens in real time.

The `on_change_callback` is used selectively, for camera parameters, it pushes the new value to the
camera driver via `camera_calibrator->upload_to_camera()`. Most other sliders don't need the callback
because the parameter is read each frame anyway.

### `Toggle`

Binds to a `bool*`. Renders as a small switch that flips when its knob is clicked. Used for
camera-side enables (auto-exposure, auto-focus) and similar binary settings.

The hit detection is circular (around the knob) instead of rectangular, because the toggle doesn't
fill its rowspace, only the knob is interactive.

## End-to-end frame flow

Putting the pieces together, here's what happens to each frame:

```
1. RosHandler delivers a raw cv::Mat
2. gui->upload_frame(frame):
     • inverse-polyline blacks out outside-ROI       [PREPROCESSING layer]
     • frame is uploaded to GPU and preprocessed
3. detector->update():
     • identification + classification run
     • detector queues overlay commands               [MARKINGS layer]
4. interface_manager->draw_interface():
     • active widgets render themselves               [INTERFACE layer]
5. gui->display_frame():
     • image downloaded from GPU
     • MARKINGS layer flushed onto the image
     • image (viewport-cropped, resized) and sidebar composed into display_buffer
     • INTERFACE layer flushed onto display_buffer
     • FPS counter drawn
     • cv::imshow
6. cv::waitKey reads any keys and dispatches mouse events
   → on_mouse → InterfaceManager::handle_input → widget callbacks or calibrator clicks
```

Every frame goes through this loop. State changes (entering a calibration mode, saving a parameter)
just modify `app_data` in place; the next frame picks up the new state and renders the new widget set.
There's no separate redraw mechanism, the display is regenerated from scratch on every frame anyway.

## Why this design

Three properties matter and the structure is built around them:

- **Mode-driven UI without a UI framework.** Calibration tools, detection, and the main menu all share
  the same canvas and the same input dispatch. Adding a new mode is "create a state, populate its
  widget vector, switch to it", no window management, no event plumbing.
- **Live parameter tuning with no save/apply step.** Sliders bind directly to the parameter structs
  the GPU pipeline reads. Moving a slider updates the next frame, which means the operator can dial in
  a value while watching its effect in real time.
- **Separation of "what to draw" from "when to draw it".** The Drawer's layered command queue lets the
  detector and the calibrators emit overlays from anywhere without caring about composition order. The
  GUI flushes layers at the right point in the frame and the visual stack is correct.

The whole interface is around 1100 lines of C++ across 13 files. It's small, but it's enough to drive
the entire calibration and tuning workflow that the rest of the project depends on.
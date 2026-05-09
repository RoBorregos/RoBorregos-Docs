# Vision

## Abstract
The vision system was implemented on two OpenMV H7 Plus cameras, programmed using the official OpenMV IDE. This platform provides a MicroPython-based environment optimized for machine vision tasks, offering built-in image processing libraries that allow efficient blob detection, color filtering, and geometric calculations directly on the camera hardware without requiring an external processor.

Color detection is based on the LAB color space, which separates luminance from color information, making it significantly more robust against lighting variations compared to RGB. Each target object — the ball, the yellow goal, and the blue goal — is defined by a calibrated LAB threshold range that isolates the target color from the background under competition lighting conditions.

All processed data is transmitted from each camera to the main microcontroller via UART communication at 115200 baud. Each frame produces a single formatted string containing the distance, angle, and area of each detected object, which is then parsed by the main controller to feed the robot's decision-making logic.

## Front Camera

The front-facing camera operates at a resolution of 440×300 pixels with fixed exposure and gain settings to ensure consistent color detection regardless of ambient lighting changes. It is responsible for detecting the ball, the yellow goal, and the blue goal in the horizontal plane directly in front of the robot.

### Color and Shape Filtering

Ball detection applies a two-stage validation combining color and shape filtering. Blobs matching the ball's LAB color threshold are first identified and sorted by area. Close-range detections — those exceeding a minimum area threshold — are additionally validated by a circularity filter, requiring a minimum roundness value to confirm the blob corresponds to a spherical object. Far-range detections, where the ball appears as a small blob, rely on color filtering alone given that shape information is insufficient at low pixel counts. This dual-range strategy improves detection reliability across the full range of distances encountered during gameplay.

Goal detection selects the largest blob matching each goal's respective LAB color threshold, applying independent pixel count and area filters to reject spurious detections. The angle of each detected object is computed relative to the image center using an arctangent function, and a distance weight is applied to the Y component of the angle calculation for the ball, compensating for perspective distortion as the ball approaches the camera.

## Mirror Camera

The mirror-facing camera processes the omnidirectional image produced by the dual mirror system. Given that the image is a circular projection of the full 360° environment, the detection pipeline is divided into two independent zones based on the physical mirror that produced each region of the image.

### Conic Mirror Zone — Far Field

The conic mirror covers the far-field region of the image, corresponding to objects at greater distances from the robot. In this zone, detected blobs are small and lack sufficient shape information for reliable circularity validation. Therefore, ball detection in the conic zone relies exclusively on color filtering using a dedicated LAB threshold calibrated for the optical characteristics of the conic mirror surface. An upper area bound is applied to prevent large spurious blobs from being incorrectly classified as ball detections.

### Ring Mirror Zone — Near Field

The ring mirror covers the near-field region, corresponding to objects in the immediate vicinity of the robot. Blobs in this zone are larger and contain sufficient shape information for additional validation. Ball detection here applies both color filtering and a circularity threshold, improving specificity and reducing false positives caused by color noise near the robot's own structure.

For both goal colors, detection across the full mirror image selects the largest blob matching each LAB threshold, applying area and pixel count filters consistent with those used in the front camera. A dead zone filter is applied to angular outputs to suppress oscillations caused by small detection noise near the zero-degree reference, and a distance correction is applied when detected objects appear in the rear arc of the mirror image to compensate for geometric distortion introduced by the mirror projection.

## Recommendations

For future iterations, it is recommended to optimize the calibration between both cameras to reduce geometric distortion, evaluate processing synchronization between the front and mirror cameras to reduce latency, and implement data fusion techniques that allow more efficient use of information from both vision sources. Additionally, documenting LAB threshold ranges per lighting environment would significantly reduce recalibration time during competition setup.
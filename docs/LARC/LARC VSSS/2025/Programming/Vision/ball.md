# Ball detection

## HSV segmentation

The ball is detected using HSV color segmentation, where the input image is converted to HSV space for robust color filtering. A predefined range (colorParams) isolates the ball's color, generating a binary mask. Noise is reduced through morphological operations (dilation + erosion), and the largest valid contour is identified as the ball.

''  Insert code here ''

## Polynomial Extrapolation

To predict the ball's movement, a moving average window stores recent positions. A 2nd-degree polynomial fit extrapolates the next position, improving tracking during fast motion. This helps compensate for processing delays, ensuring smoother coordinate updates.
 
''Include code here''

## Coordinate extrapolation

Detected ball positions are transformed from pixel coordinates to real-world field coordinates using a homography matrix, enabling accurate positioning for robot navigation.

''Include code here''

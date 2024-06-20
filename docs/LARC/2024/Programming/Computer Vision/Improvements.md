## Closest Cube Detection

Since a 3D camera is not used, measuring distances is not available. In order to do this, a camera that has a degree of 45o was used. Now the items nearest to the bottom “y” margin can be known, as well as which one is closer. 
## Blocking a Cube 
When it comes to following a specific object in an image, it is important to understand how this object changes through time and images. A square margin was implemented so it would be surrounding the contour of the color cube detection or ArUco, and compared in the next image if there was a cube with the same ID, color or code, inside the margin. This allows us to prevent the robot from following multiple cubes at the same time. This will also prevent the robot from getting confused with the same ID cubes, since they can have the same ID. However, it is rare in this scenario for there to be in the same margin as the one that has already been declared. 

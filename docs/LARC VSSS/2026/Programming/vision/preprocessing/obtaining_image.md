# Obtaining an image

The first step of the vision pipeline is to obtain an image to work in. Depending on the parameters specified in the 
launch file is where the image will come from.

## Obtaining a frame

### Image from camera
If the `use_camera` flag is set to true, the program will open the device in `camera_id` and receive images.

### Image from simulation
If the `simulation` flag is set to true, the ros handler library will fetch the latest image in the 
`vision/sim_image` topic.
The ros handler library receives images and stores them even if they are not consumed, the intention is that 
whenever the program needs a frame, it can be obtained.

### Image from file
If `simulation` and `use_camera` are both set to false, the program will attempt to get a frame from the file. The 
supported file formats are `.mp4`, `.avi`, `.mov`, `.mkv`

## Frame journey
The frame journey looks like this:

1. Frame is obtained through any method
2. The frame is passed to the [graphical interface](../graphical_interface/index.md)
3. The frame is uploaded to the GPU 
4. Filters are applied, see [filters](./filters.md) to learn more about the filter pipeline
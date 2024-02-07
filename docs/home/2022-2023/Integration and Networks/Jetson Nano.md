# Jetson Nano Setup

With jetson nano we use the Ubuntu 20.04 image, you can download it from [here](https://github.com/Qengineering/Jetson-Nano-Ubuntu-20-image.git). You need to flash directly the image to the sd card, you can use [balenaEtcher](https://www.balena.io/etcher/) to do it.

## Considerations

- You need to have a 32GB sd card and the fixed image is of this size, to get more space you only need to expand the partition with gparted or any other tool.
- Teamviewer is already installed in the image, but the repository is not updated, so you can errase it in order to get a good output from apt update.
- Cuda is already installed in the image, you can check it with the following command:

```bash
nvcc --version
```
if is not detected remember to fix the path with the following command:

```bash
export PATH=/usr/local/cuda-<Version>/bin${PATH:+:${PATH}}
```
- If you want to intall the Zed SDK you need to check the cuda version and install the correct version of the SDK
- If you want to install ROS noetic you can follow the instructions [here](http://wiki.ros.org/noetic/Installation/Ubuntu)


## Hardware considerations

- You need to have a 5V 4A power supply, but you can also use 2 power supplies of 5V 3A. with to the corresponding pins in the board. 
- Fan is not included in the board, you can use a 5V fan and connect it to the 5V and GND pins in the board or you can use a generic 12V fan of 2 inches and connect it to the an external power supply.
- You can use a wifi dongle to connect the board to the internet, some of them are already compatible with the board, but if is not detected you should check the model and install the drivers for linux. Some of them are not compatible with the board, so you should check the compatibility before buying one.
- Sometimes usb ports are not working, consider this ports or change the jetson nano board.
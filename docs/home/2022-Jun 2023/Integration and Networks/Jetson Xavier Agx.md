# Jetson AGX Xavier

To flash the Jetson AGX Xavier, you need to download the [JetPack SDK](https://developer.nvidia.com/embedded/jetpack) from Nvidia. The JetPack SDK is a collection of software tools that enables the development of AI applications for Jetson. It includes the BSP (Boot Software Package), Linux kernel, NVIDIA CUDA, and TensorRT libraries for deep learning, computer vision, GPU computing, multimedia processing, and much more.

## Flashing the Jetson AGX Xavier

1. Download the JetPack SDK from Nvidia. You will need to create an account to download the SDK in [Nvidia Developer](https://developer.nvidia.com/drive/downloads).
2. Put the jetson AGX Xavier on recovery mode. There is a recovery button on the board, which is in the middle of three buttons. Hold the recovery button and then power it up, which will enter the Force Recovery Mode
3. You should connect the Jetson AGX Xavier to your computer with a USB to USB-C cable. The USB-C port is the one that is closer to the power button. It should detect the board![sdk Manager ](/assets/home/sdk0.png)
4. You Can select the components you want to install. We recommend to install all of them.![sdk Manager ](/assets/home/sdk1.png)
5. Then you should flash the board (if you are using SSD storage you should change the Storage Device).![sdk Manager ](/assets/home/sdk2.png)
6. When you get to install additional components, you will get a promt, at that moment the board has already a initial OS, if you get an error in the connection change to another method and check with a display the real IP of the board.![sdk Manager ](/assets/home/sdk3.jpeg)

## Setting up the Jetson AGX Xavier

At this moment you should have an ubuntu 20.04 OS in the Jetson AGX Xavier. You can connect the board to a display and a keyboard or you can also connect it to the internet with an ethernet cable or with a wifi dongle.

Some basic instructions to set up the board:

- [ROS noetic](http://wiki.ros.org/noetic/Installation/Ubuntu) installation
- [Zed SDK](https://www.stereolabs.com/docs/installation/jetson/) installation
  
### Misc

There are some issues working with the Jetson AGX Xavier, here are some of them:

#### Visual Studio Code

To run visual studio code you should get an ARM installer, if you cant launch it try with the following command:

```bash
code --no-sandbox
```

#### CUDA 

Check if cuda is correctly installed with the following command:

```bash
nvcc --version
```
if not, you can check if you are only missing the links to cuda with the following command:

```bash
export PATH=/usr/local/cuda-12.0/bin${PATH:+:${PATH}}
```
**Remember to change the cuda version if you are using another one.**

#### Zed SDK

Check which version of Zed SDK you are installing acording to your cuda version.






# OpenMV

OpenMV is a project that aims to provide low-cost and easy-to-use machine vision modules. The OpenMV cameras run on MicroPython (Python 3), and can be programmed using the OpenMV IDE that facilitates development and contains additional tools.

- Camera used: [Openmv H7 R2](https://openmv.io/products/openmv-cam-h7-r2)
- Documentation: [Openmv Documentation](https://docs.openmv.io/)

## RPC

>The rpc module on the OpenMV Cam allows you to connect your OpenMV Cam to another microcontroller or computer and execute remote python (or procedure) calls on your OpenMV Cam. The rpc module also allows for the reverse too if you want your OpenMV Cam to be able to execute remote procedure (or python) calls on another microcontroller or computer.

-[Openmv Docs](https://docs.openmv.io/library/omv.rpc.html)

See the [script](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/maze_ws/src/devices/openmv_camera/scripts/camera_script.py) used in the OpenMVs to detect colors and send images through RPC.

See the code used to make calls to OpenMVs and interpret camera results [here](https://github.com/RoBorregos/rescuemaze-2023/blob/pidrotation/maze_ws/src/devices/openmv_camera/scripts/camera_controller.py).



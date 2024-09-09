# Network

The comunication between the different devices is done with ROS network. You should set a ROS master in one of the devices and the rest of the devices should connect to it.

## Master machine 

The master machine is the one that has the ROS master. It can be any of the devices, but we recommend to use the Jetson Xavier AGX so everyone can use the same ROS master.
before starting the ROS master you should set up the ROS network with the following commands:

```bash
export ROS_MASTER_URI=http://<ROS_MASTER_IP>:11311
export ROS_IP=<ROS_MASTER_IP>
```
<small>*We recommend to add this commands to the .bashrc file so you dont have to type them every time you open a new terminal. *</small>

## Slave machines

The slave machines are the ones that connect to the ROS master. Remember to also set up your ip of the local device in every terminal
    
    ```bash
    export ROS_MASTER_URI=http://<ROS_MASTER_IP>:11311
    export ROS_IP=<ROS_SLAVE_DEVICE_IP>
    ```
<small>*We recommend to add this commands to the .bashrc file so you dont have to type them every time you open a new terminal. *</small>

## Misc

In the DashGo Robot there is a modem that creates a wifi network, you can connect to it, also everything that is in the robot is connected with ethernet cables to a switch and then to the modem. You can connect to the switch with an ethernet to have a better connection. but you can also connect to the modem wifi network.

### Devices 

#### SSH Connection

You can connect to the devices with ssh, you can use the following command:

```bash
ssh <user>@<ip>
```
The user for Jetson Xavier AGX is *nvidia* and the default password is *nvidia*
The user for Jetson Nano is *jetson* and the default password is *jetson*

We recommend to use visual studio code to connect to the devices, you can install the [Remote Development Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.vscode-remote-extensionpack) and then connect to the devices with the following command:

#### Jetson Xavier nano 
For jetson nano you can connect with a micro usb cable, it should be detected as ethernet connection, you can connect to it with the following command:

```bash
ssh jetson@192.168.55.1
```
<small>The default ip for jetson nano is 192.168.55.1 when a micro usb cable is connected</small>

#### Display

Remember you have a small display to do some basic things in the jetson xaiver agx, you can also connect a keyboard and a mouse to it to make fast configuration changes like audio, wifi, etc.

### Troubleshooting

#### Not listening topics

If you are having problems with the ROS network, you can check if the ROS master is listening the topics with the following command:

```bash
rostopic list
```
Sometimes it will show you the topics but it will not be listening them, you can check if the ROS master is listening the topics with the following command:

```bash
rostopic info <topic_name>
```

If the topic is not listening, you can try to restart the ROS master with the following command:
set again the ROS networ, master and your device ip and then restart the ROS master

#### Unable to communicate with master

Be sure that you have set up the ROS network correctly, and you are running the ROS master in the master machine. If you are still having problems, you can try to restart the ROS master

#### Nothing appears in terminal

When you run something and it's not properly set check if you are connected to the same network, you can check you ip with the following command:

```bash
ifconfig
```

#### Multiple WiFi near

When you have multiple WiFi networks near, you can have problems with network, you should check the best channel for your network and set it up in the modem. If needed you can also connect to the switch with an ethernet cable to have a better connection.

#### Slow data transfer

In many cases you should transfer data between devices, always be aware of the best way to do it, images, pointclouds and other big data can get you slow frecuencies in topics, you should try to keep all the Real Time processes in the jetson xavier agx and the jetson nano, and the other devices should only be used as debug devices or really large processing devices.
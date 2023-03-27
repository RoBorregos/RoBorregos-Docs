# Running Project on Jetson nano

## Launching files

- Initialize sensors, hector slam, move base, and services:

```bash
roslaunch nav_main launch_jetson.launch
```

- Run navigation algorithm:

```bash
roslaunch exploration main
```

- Use rviz from external computer:

```bash
export ROS_IP=YOUR_IP
export ROS_MASTER_URI=JETSON_IP

rosrun rviz rviz -d $(rospack find robot_description)/rviz/urdf.rviz
```


## Connecting to Jetson using SSH

- Obtain jetson's IP
- Perform SSH
- Introduce password

```bash
sudo nmap -sn YOUR_IP/24
ssh username_jetson@JETSON_IP
```

## Debug using teleop

- Make sure ROS_IP and ROS_MASTER_URI are properly set if using another laptop.

```bash
rosrun teleop_twist_keyboard teleop_twist_keyboard.py _speed:=0.8 _turn:=2.4 _repeat_rate:=10
```

## Add new files to jetson

If new code was implemented outside of the jetson, the file(s) can be copied using the following command:

```bash
# use -r (recursive flag) for folders
scp -r SOURCE/ DESTINATION/
# e.g pass files from laptop to jetson (run command on laptop terminal with ssh connected to jetson)
scp -r /home/oscar/maze_ws/src/devices/ jetson@IP:/home/jetson/maze_ws/src/
```

Also, you may want to consider deleting the files from the jetson first before using scp with the new files:

```bash
# e.g. deleting devices folder before scp
# in jetson
rm -rf /home/jetson/maze_ws/src/devices
```

Finally, use catkin_make to apply changes.

```bash
cd ~/maze_ws
catkin_make
``` 

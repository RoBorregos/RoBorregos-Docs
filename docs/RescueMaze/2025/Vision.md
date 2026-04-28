
# Vision

To detect letter and color victims, a Jetson Nano is used along with CSI cameras, TensorFlow, OpenCV, and a MobileNet model in Python.  
To avoid misidentification, the ESP32 specifies whether the detection comes from the right or left camera, depending on the presence of a wall.

## Communication: ESP32 <-> Jetson

To send information between the ESP32 and the Jetson, serial communication is used with the following structure:

The protocol uses 8 bytes:

1. **Byte 0:** HEADER0  
2. **Byte 1:** HEADER1  
3. **Byte 2:** Packet size  
4. **Byte 3:** Command  
5. **Bytes 4–7:** 4-byte integer for additional parameters

The protocol uses headers to ensure safe sending and receiving of information. The packet size indicates the length of the message, the command specifies the required action (e.g., return detection result from right or left camera), and the 4-byte parameter can be used for additional configurations (e.g., initializing at 115200 baud).

![Vision_diagram](/assets/maze/Vision_diagram.png)

## TensorFlow Model Architecture

The model is based on Google’s MobileNet, trained using transfer learning.

- Model used: [MobileNet v2 feature vector](https://tfhub.dev/google/tf2-preview/mobilenet_v2/feature_vector/4)  
- Dependencies: [requirements.txt](https://github.com/RoBorregos/robocup-rescuemaze-2025/blob/Vision/requirements.txt)

### Training Recommendations

- Use an `ImageDataGenerator` with high variability in conditions (lighting, angles, etc.)
- Split data into training and test sets
- The number of epochs depends on the dataset size. Example: 150 epochs for 750 images per class to avoid overfitting

### Overfitting

Overfitting occurs when a model learns the training data too well, including noise and irrelevant patterns. This results in high performance on training data but poor generalization to new, unseen data.

## Datasets

The dataset consists of a folder containing subfolders for each class:

```
/dataset/
├── H/
│   ├── H1.jpg
│   ├── H2.jpg
├── S/
│   ├── S1.jpg
│   ├── S2.jpg
├── U/
│   ├── U1.jpg
│   ├── U2.jpg
├── NONE/
│   ├── NONE1.jpg
│   ├── NONE2.jpg
```

The accuracy of the model strongly depends on the diversity and size of the dataset used during training.

## Color Detection

(*This section is currently empty. Consider adding a description of the color detection logic, tools used, and its integration with the pipeline.*)

## Jetson Debugging

To access the Jetson terminal without a monitor, SSH is used:

```bash
ssh deviceName@deviceIP
```

To visualize images or GUIs via SSH, use `Xming`:

1. Download and install **Xming**
2. Run **XLaunch** and configure it to launch at startup
3. In the SSH session, run:
   ```bash
   export DISPLAY=PC_IP
   export LIBGL_ALWAYS_INDIRECT=1
   ```
4. Launch your visualization/debugging tools

## Executable Setup

Running TensorFlow and the camera script simultaneously can cause conflicts. To avoid this, `tmux` is used to run each script in a separate terminal window:

```bash
# tmux session name
SESSION="maze_session"

# Create new session
tmux new-session -d -s $SESSION

# Panel 0: camera script
tmux send-keys -t $SESSION "export LD_PRELOAD=/usr/lib/aarch64-linux-gnu/libGLdispatch.so.0 && python3 /home/jetson/maze/com/load_cams.py" C-m

# Split window vertically
tmux split-window -v -t $SESSION

# Panel 1: model script
tmux send-keys -t $SESSION "python3 /home/jetson/maze/com/test.py" C-m

# Attach to session
tmux attach-session -t $SESSION
```

To run everything:
```bash
./run_all.sh
```

To close the session, first press `Ctrl+C` in each terminal, then run:

```bash
tmux kill-session -t maze_session
```


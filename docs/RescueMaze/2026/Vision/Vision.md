# Vision System - 2026 Documentation

## Overview

The Vision subsystem runs on a **Raspberry Pi 5** with dual **IMX219 CSI cameras** connected via Picamera2 and YOLO inference. It detects victims (harmed/unharmed/stable) and targets (bullseye patterns) in real-time, communicating with the ESP32 over UART at 115200 baud.

**Key Hardware:**
- Raspberry Pi 5
- Dual IMX219 CSI cameras (1920x1440 native resolution)
- UART serial connection to ESP32 (via GPIO pins 14/15)
- Python 3.10+ with Picamera2, OpenCV, PyTorch/YOLO runtime

---

## Communication Protocol

### UART Configuration
- **Port:** `/dev/serial0` (GPIO-based serial) or `/dev/ttyACM0` (USB serial)
- **Baud Rate:** 115200
- **Timeout:** 0.5s
- **Data Format:** 6-byte binary packets with header, length, payload, checksum

### Packet Structure

| Byte | Value | Description |
|------|-------|-------------|
| 0 | 0xFF | Header start |
| 1 | 0xAA | Header end |
| 2 | payload_len | Length of payload, max 32 |
| 3 | payload[0..n-1] | Actual payload |
| n | checksum | XOR of bytes 2 through n-1 |

### Detection Request (ESP32 - Raspberry Pi)
ESP32 sends a detection request with:
- **Command:** `0x01` (detection request)
- **Camera ID:** `0x00` (RIGHT) or `0x01` (LEFT)

**Example packet:** `FF AA 02 01 00 03`

| Field | Value | Meaning |
|-------|-------|----------|
| Payload | [0x01, 0x00] | command=request, camera=RIGHT |
| Checksum | 0x03 | 0x02 ^ 0x01 ^ 0x00 = 0x03 |

### Detection Response (Raspberry Pi - ESP32)
Raspberry Pi responds with detected victim type:
- **Command:** `0x02` (detection response)
- **Camera ID:** `0x00` or `0x01`
- **Victim ID:** See table below

**Victim Type Codes:**
| ID | Name | Meaning |
|----|------|---------|
| 0x00 | VICTIM_NONE | No victim detected (bullseye/background) |
| 0x01 | VICTIM_PHI (Φ) | Harmed victim (red marker) |
| 0x02 | VICTIM_PSI (Ψ) | Unharmed victim (green marker) |
| 0x03 | VICTIM_OMEGA (Ω) | Stable victim (yellow marker) |
| 0x04 | VICTIM_HARMED | Alt. harmed classification |
| 0x05 | VICTIM_UNHARMED | Alt. unharmed classification |
| 0x06 | VICTIM_STABLE | Alt. stable classification |
| 0x07 | VICTIM_FAKE_TARGET | Bullseye without victim |

**Example response:** `FF AA 03 02 00 01 02`

| Field | Value | Meaning |
|-------|-------|----------|
| Payload | [0x02, 0x00, 0x01] | response, camera=RIGHT, victim=PHI/harmed |
| Checksum | 0x02 | 0x03 ^ 0x02 ^ 0x00 ^ 0x01 = 0x02 |

---

## Raspberry Pi Hardware Setup

### 1. Operating System & Dependencies
```bash
# Use Raspberry Pi OS Lite 64-bit (bookworm recommended)
# Install system packages
sudo apt update
sudo apt install -y python3-dev python3-venv python3-pip
sudo apt install -y libopencv-dev python3-opencv
sudo apt install -y libatlas-base-dev libjasper-dev libhdf5-dev libharfbuzz0b libwebp6
sudo apt install -y libopenjp2-7 libtiff6 libjasper1 libharfbuzz-dev
```

### 2. Serial Port Configuration

Enable UART on GPIO pins 14/15 with these steps:

| Step | Command/Action | Details |
|------|---|---|
| 1 | `sudo raspi-config` | Open configuration tool |
| 2 | Interface Options | Navigate to Interface Options |
| 3 | Serial Port | Select Serial Port |
| 4 | Login shell over serial? | Choose No |
| 5 | Serial port hardware? | Choose Yes |
| 6 | Enable UART hardware | Confirm |

Verify and test:

```bash
# Verify port is available
ls -la /dev/serial0
# Should output: /dev/serial0 -> ttyAMA0

# Test connection
sudo picocom -b 115200 /dev/serial0
```

### 3. Virtual Environment & Packages
```bash
cd /path/to/robocup-rescuemaze-2026

# Create Python venv
python3 -m venv vision_env
source vision_env/bin/activate

# Install dependencies
pip install --upgrade pip setuptools wheel
pip install pyserial picamera2 opencv-python ultralytics torch torchvision
pip install numpy scipy pillow

# Verify installation
python3 -c "import picamera2; import cv2; import ultralytics; print('All imports OK')"
```

### 4. Camera Configuration (Picamera2)
```bash
# List available cameras
libcamera-hello --list-cameras

# Expected output:
# 0 : imx219 [1640x1232] 
# 1 : imx219 [1640x1232]

# Check camera FoV & focus
libcamera-hello -t 5 --camera 0
libcamera-hello -t 5 --camera 1
```

---

## YOLO Model & Detection Pipeline

### Model Architecture
- **Framework:** YOLOv8 (Ultralytics)
- **Model Weights:** `Vision/weights/best.pt` or `best_maze_model.pt`
- **Input Resolution:** 640×640 (inference size, resized from 1920×1440 camera capture)
- **Inference Device:** CPU (Raspberry Pi) or TPU (optional accelerator)

### Detection Classes
The model is trained to recognize:
1. **Victims:** Harmed (PHI/Φ, red), Unharmed (PSI/Ψ, green), Stable (OMEGA/Ω, yellow)
2. **Targets:** Bullseye patterns (yellow circles with black rings)
3. **False positives:** Distinguish victim markers from environmental clutter

### VisionDetector Class (`Vision/detector.py`)
```python
from detector import VisionDetector

# Initialize detector
detector = VisionDetector()

# Single camera detection
victim_id = detector.detect_victim(camera_id=0)  # camera_id: 0=RIGHT, 1=LEFT

# Combined detection (target first, victim fallback)
victim_id = detector.detect_combined(camera_id=0)

# Clean up
detector.close()
```

**Detection Flow:**
1. Capture frame from specified camera (1920×1440 @ Picamera2)
2. Resize & convert to BGR for OpenCV
3. Run YOLO inference (640×640 network)
4. Extract top-confidence detection
5. Map YOLO class ID to victim ID (0-7)
6. Return victim ID or VICTIM_NONE if confidence < threshold

### Performance Metrics
| Metric | Value |
|--------|-------|
| Inference time (CPU) | ~150-200ms per frame |
| Frame rate | ~5-7 FPS (single camera) |
| Model size | ~22MB (best.pt) |
| Confidence threshold | 0.50 (tunable in Constants.py) |
| IOU threshold | 0.10 (non-max suppression) |

---

## Configuration Tuning (`Vision/Constants.py`)

### Camera Settings
```python
# Physical camera mapping
camera_right_index = 0          # CSI camera 0 (physically on right side)
camera_left_index = 1           # CSI camera 1 (physically on left side)

# Picamera2 resolution (per-camera)
vision_picamera_right_width = 1920   # Native IMX219 width
vision_picamera_right_height = 1440  # Native IMX219 height
vision_picamera_left_width = 1920
vision_picamera_left_height = 1440

# Tuning file (if needed for fisheye distortion)
vision_picamera_tuning_file = "/usr/share/libcamera/ipa/rpi/pisp/imx219_noir.json"

# Color order (adjust if colors look wrong)
vision_picamera_color_order = "BGR"  # Use "BGR" for OpenCV compatibility
```

### Inference Settings
```python
vision_conf_threshold = 0.50        # Min confidence to report detection
vision_imgsz = 640                  # YOLO inference size
vision_iou_threshold = 0.10         # Non-max suppression threshold
vision_force_frame_size = False     # Keep native Picamera2 size when False
```

### Per-Camera Color Correction
```python
# RIGHT camera gain adjustment (for better yellow/green separation)
vision_right_gain_b = 1.00
vision_right_gain_g = 0.92
vision_right_gain_r = 1.06
vision_right_saturation_scale = 1.10

# LEFT camera (neutral defaults)
vision_left_gain_b = 1.00
vision_left_gain_g = 1.00
vision_left_gain_r = 1.00
vision_left_saturation_scale = 1.00
```

### Target Detection Tuning
```python
target_fast_mode = True              # Skip expensive contour checks
target_circle_min_circularity = 0.30 # How round the bullseye must be
target_circle_border_allow_ratio = 0.08  # Edge tolerance for fisheye
target_ring_n_angles = 56            # Number of ring angle samples
```

**Tuning Tips:**
- If victims are over-detected: increase `vision_conf_threshold` (e.g., 0.60)
- If victims are under-detected: decrease threshold or adjust per-camera gains
- If targets are noisy: increase `target_circle_min_circularity` or adjust `target_roi_pad_ratio`
- For fisheye lenses: increase `target_circle_border_allow_ratio` (0.10-0.15)

---

## Execution Modes

### 1. Victim-Only Detection (Default)
```bash
cd Vision
source ../vision_env/bin/activate
python3 run_uart.py --mode victim
```
- Listens on `/dev/serial0` at 115200 baud
- Responds to detection requests with victim type only
- Use for primary maze detection

### 2. Target-Only Detection
```bash
python3 run_uart.py --mode target
```
- Detects bullseye patterns only
- Ignores victim markers (red/green/yellow)
- Use for target identification tasks

### 3. Combined Detection (Target - Victim Fallback)
```bash
python3 run_uart.py --mode combined
```
- Attempts target detection first
- Falls back to victim detection if target is NONE
- Use for missions requiring both target and victim identification

### 4. Direct Test Mode (No UART)
```bash
python3 detector.py --camera 0 --show
```
- Captures and displays live frames with bounding boxes
- Useful for debugging camera calibration and model performance
- Press 'q' to exit

### Systemd Service (Persistent)
```bash
# Create service file
sudo tee /etc/systemd/system/vision.service > /dev/null <<EOF
[Unit]
Description=RoboCup Vision Detection Service
After=network.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/robocup-rescuemaze-2026/Vision
ExecStart=/home/pi/robocup-rescuemaze-2026/vision_env/bin/python3 run_uart.py --mode victim
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

# Enable and start
sudo systemctl daemon-reload
sudo systemctl enable vision
sudo systemctl start vision

# View logs
sudo journalctl -u vision -f
```

---

## Integration with ESP32 Maze Algorithm

### ESP32 Detection Request Flow (from `include/raspy.h` & `src/raspy.cpp`)

**Constants:**
```cpp
#define DETECTION_ATTEMPTS 1              // Single attempt for speed
#define DETECTION_MIN_CONSENSUS 1         // No vote needed (1 attempt)
#define DETECTION_ATTEMPT_DELAY_MS 10     // Minimal inter-request delay
#define waitingTime 300                   // 300ms timeout per camera

// UART Protocol (same as Python side)
#define UART_HEADER0 0xFF
#define UART_HEADER1 0xAA
#define CAM_RIGHT 0x00
#define CAM_LEFT 0x01
#define CMD_REQUEST 0x01
```

**Detection Logic:**
```cpp
// From Raspy::getDetection() in src/raspy.cpp
uint8_t Raspy::getDetection() {
  uint8_t right_results[DETECTION_ATTEMPTS] = {0};
  uint8_t left_results[DETECTION_ATTEMPTS] = {0};
  
  // Request from both cameras
  for (int i = 0; i < DETECTION_ATTEMPTS; i++) {
    right_results[i] = getVictim(CAM_RIGHT);  // Send 0xFF 0xAA 0x02 0x01 0x00 ...
    delay(DETECTION_ATTEMPT_DELAY_MS);
    left_results[i] = getVictim(CAM_LEFT);    // Send 0xFF 0xAA 0x02 0x01 0x01 ...
    delay(DETECTION_ATTEMPT_DELAY_MS);
  }
  
  // Consensus voting (most common result)
  // If DETECTION_ATTEMPTS=1, just returns the single result
  uint8_t consensus_right = findConsensus(right_results, DETECTION_ATTEMPTS);
  uint8_t consensus_left = findConsensus(left_results, DETECTION_ATTEMPTS);
  
  // Prioritize LEFT if both have victim, else RIGHT
  return (consensus_left != VICTIM_NONE) ? consensus_left : consensus_right;
}
```

**Timing:** ~620ms total round-trip (RIGHT 300ms + LEFT 300ms + overhead)

### In Main Loop
```cpp
// From src/main.cpp
void loop() {
  // During maze traversal
  uint8_t victim_type = raspy.getDetection();
  
  if (victim_type != VICTIM_NONE) {
    // Mark victim location in maze map
    maze.addVictim(current_x, current_y, victim_type);
    
    // Log to screen
    motors.screenPrint("VICTIM: ");
    motors.screenPrint(victim_name(victim_type));
  }
  
  // Continue maze exploration
  delay(10);
}
```

---

## Debugging & Logging

### SSH Access & Real-Time Monitoring
```bash
# SSH into Raspberry Pi
ssh pi@192.168.1.X

# Monitor vision service
tail -f /var/log/syslog | grep vision

# Or use journalctl for systemd service
journalctl -u vision -f

# Check serial activity
cat /dev/serial0 &
# Then send ESP32 requests manually (simulate with serial monitor)
```

### Manual UART Test
```bash
# Terminal 1: Listen on serial
sudo picocom -b 115200 /dev/serial0

# Terminal 2: Send test packet (0xFF 0xAA 0x02 0x01 0x00 0x03)
# Using Python
python3 << 'EOF'
import serial
port = serial.Serial('/dev/serial0', 115200)
port.write(bytes([0xFF, 0xAA, 0x02, 0x01, 0x00, 0x03]))  # Request RIGHT camera
print(port.read(6).hex())  # Should read ~6 byte response
EOF
```

### Model Inference Profiling
```bash
# Time individual frames
python3 << 'EOF'
import time
from detector import VisionDetector

detector = VisionDetector()
for i in range(10):
    start = time.time()
    victim_id = detector.detect_victim(camera_id=0)
    elapsed = time.time() - start
    print(f"Frame {i}: {elapsed*1000:.1f}ms - {victim_id}")
detector.close()
EOF
```

### Vision Logs (via `print()` statements)

| Log Message | Meaning |
|---|---|
| `[RX] FF AA 02 01 00 03` | Received detection request from ESP32 |
| `[REQ] len=2 payload=[1, 0]` | Parsed request (cmd=1, cam=0) |
| `[SENT] CAM=RIGHT VICTIM=PHI` | Detected victim, sending response |
| `[TX] FF AA 03 02 00 01 02` | Sent response packet |

---

## Dataset & Model Training Notes

### Training Data

| Aspect | Details |
|---|---|
| Source | RoboCup Rescue Maze 2024-2025 arena captures |
| Labels | Victim markers (red/green/yellow) + bullseye targets |
| Image count | ~2000 labeled frames (per camera calibration) |
| Augmentation | Brightness ±10%, rotation ±5°, blur, noise |

### Model Files

| File | Description |
|---|---|
| `best.pt` | Latest trained YOLOv8 model (PyTorch format, ~22MB) |
| `best_maze_model.pt` | Alternative maze-specific model |
| `best.onnx` | ONNX export (for potential TensorFlow Lite deployment) |

### Retraining
```bash
from ultralytics import YOLO

# Load pretrained model
model = YOLO('yolov8n.pt')  # Nano model

# Train on custom dataset
results = model.train(
    data='dataset.yaml',      # Custom YAML with image paths & classes
    epochs=50,
    imgsz=640,
    device=0,                 # GPU ID or 'cpu'
    batch=16,
    patience=10,
)

# Validate & export
model.val()
model.export(format='pt')     # Save best.pt
model.export(format='onnx')
```

---

## Performance Tuning for Raspberry Pi

### CPU Optimization
1. **Reduce inference resolution:** Change `vision_imgsz` from 640 to 416 (faster, less accurate)
2. **Enable model quantization:** Convert best.pt to INT8 (25% faster)
3. **Use model pruning:** Reduce YOLO model depth (yolov8n = nano, vs yolov8s = small)

### Thermal Management
```bash
# Check CPU temperature
vcgencmd measure_temp

# Boost performance (if cooling available)
echo powersave | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Or: performance (but runs hot)
```

### Memory Usage
```bash
# Check available RAM
free -h

# Lighten payload if RAM-constrained
# Option 1: Single camera at a time (halve memory)
# Option 2: Lower inference resolution (imgsz=416)
# Option 3: Enable GPU acceleration if available (Compute Module 4 with GPU)
```
# Main problems

| Problem | Cause | Solution |
|---|---|---|
| Camera color mismatch | BGR/RGB conversion and different lighting between cameras | Tune `vision_picamera_color_order` and per-camera gain values |
| False victim detections | Low confidence threshold or noisy frames | Increase `vision_conf_threshold` and adjust YOLO tuning parameters |
| High inference latency | Running YOLO on Raspberry Pi CPU | Reduce `vision_imgsz`, use a lighter model, or improve thermal conditions |
| Serial communication issues | UART misconfiguration or disconnected wiring | Verify `/dev/serial0`, baud rate, and GPIO 14/15 connections |
| Different behavior between cameras | Uneven exposure and lens response | Apply separate right/left calibration values in `Constants.py` |
| Service startup failures | Missing Python packages or environment mismatch | Check the virtual environment and reinstall the required dependencies |

---

## Troubleshooting

| Issue | Diagnosis | Solution |
|-------|-----------|----------|
| No UART response | `ls /dev/serial0` missing | Run `sudo raspi-config` - Interface Options - Serial |
| Serial timeout | UART not connected to ESP32 | Check GPIO 14 (TX) - ESP32 RX, GPIO 15 (RX) - ESP32 TX |
| Inference too slow | Model runs on CPU only | Optimize: reduce imgsz, use nano model, or add GPU accel. |
| Camera not detected | Picamera2 init fails | Run `libcamera-hello --list-cameras` to confirm CSI bus |
| Victim over-detected | Confidence threshold too low | Increase `vision_conf_threshold` in Constants.py |
| Colors look wrong | BGR/RGB mismatch | Try `vision_picamera_color_order = "RGB"` |
| Service won't start | Python import error | Run `pip list` to verify all packages installed |
| High latency (>1s) | Single frame taking too long | Check CPU temp, reduce batch size, or use nano model |

---

## Summary

| Aspect | Details |
|---|---|
| Architecture | Dual IMX219 CSI cameras - Picamera2 capture - YOLO inference (YOLOv8) - UART response to ESP32 |
| Protocol | 6-byte binary packets (0xFF 0xAA header, XOR checksum, 115200 baud) |
| Performance | ~150-200ms inference per frame, 5-7 FPS, <300ms timeout per request |
| Tuning | Constants.py for thresholds, per-camera color gains, and model parameters |
| Integration | ESP32 calls `raspy.getDetection()` - sends UART request - receives victim ID - updates maze map |
| Deployment | Systemd service for persistent operation, SSH debugging via journalctl logs |

**Next Steps**
1. Retrain best.pt on updated arena camera angles & lighting conditions
2. Calibrate per-camera color gains for your fisheye setup (if using different lenses)
3. Optimize constants (DETECTION_ATTEMPTS, waitingTime) for your maze layout
4. Test end-to-end on actual robot before competition

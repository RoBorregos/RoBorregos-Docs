# Pattern detection

The system employs a custom-trained YOLO (You Only Look Once) model specifically designed to recognize each robot's unique visual patterns on the playing field.

## Dataset generation

A dedicated automated pipeline (from HOME) generated synthetic training images by superimposing robots onto diverse backgrounds. This approach ensured robust generalization by exposing the model to: varied textures and colors, simulated lighting conditions, and multiple viewing angles.

## Model Training

The model was trained using a combination of synthetic and real-world images, enhanced with data augmentation techniques like rotations and brightness adjustments to improve detection reliability. 
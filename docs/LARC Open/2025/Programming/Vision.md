# Vision Model
## Hector Tovar & Dana Torres

To ensure the collection and separation of beans were secure, we used a YOLOv8n trained model. This differentiated us from the other teams.

### Taking Pictures
1. Use the camera you will use during the competition. This ensures the photo quality matches the competition setup; it is harder when taking pictures, but it increases reliability.
2. Following the previous point, take photos with the camera installed on the robot to maintain consistent position and reduce blur. For this step, we moved the robot manually and took photos in parallel.
3. In computer vision, more photos generally improve precision and reliability. We took around 400 photos for the competition.
4. Bean color can vary during the competition; be prepared to take new photos. On the first day, use the time for calibration: take photos, label them, and train the model.

### Labeling
Labeling is the process of telling the model what objects are in each image.

We used five labels:
1. Inmature
2. Mature
3. Overmature
4. Red_Benefit
5. Blue_Benefit

For labeling, we recommend using Roboflow.

### Using Roboflow
Create a project using the instance segmentation option. Upload the dataset of photos you took. Once uploaded, images appear as a batch you can label yourself or split among other people.

Use the interface to select each object to label. You can use Smart Select (press "S") to speed up labeling. After finishing, click the purple checkmark, then choose "Split images between Train/Valid/Test" with 70% train, 20% validation, and 10% test.

After that, go to Dataset and click Train Model -> Custom Training. You can skip the processing step.

### Augmentation
Augmentation improves model accuracy under varying lighting. We focused on two augmentations: Saturation and Brightness. Set Saturation between -25% and +25% and Brightness between -32% and +32%.

### Creation of the dataset
Set the Maximum Version Size to 3x to increase the number of images and improve model accuracy. After creating the dataset, click Download Dataset, choose the YOLOv8 format, and select Show Download Code — you will need this for training.

### Training
See the Jupyter Notebook model.ipynb in the vision folder of the GitHub repo.

Training requires Ubuntu 22.04 and CUDA installed.

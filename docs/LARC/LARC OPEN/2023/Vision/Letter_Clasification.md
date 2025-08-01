## Tensorflow Litte

Tensorflow lite is a framework for running tensorflow models on mobile and embedded devices. It enables on-device machine learning inference with low latency and a small binary size. 

We used Tensorflow lite because is a easy way to train a funcional model and it don't need a lot of resources to run, so is more efficient than a normal tensorflow model.

### Installation

To install Tensorflow Litte you need to run the following command:

```bash
pip install tflite-model-maker
```
### Datastet structure
![dataset structure](/assets/LARC/dataset.png)

### Usage

#### Train a model


##### Import the required packages

```python
import os

import numpy as np

import tensorflow as tf
from tflite_model_maker import image_classifier
from tflite_model_maker.image_classifier import DataLoader

import matplotlib.pyplot as plt
```

##### Load the dataset

```python
data_set = DataLoader.from_folder(ds_path)
```

##### Split the dataset

```python
train_data, test_data = data_set.split(0.9)
test_data, val_data = test_data.split(0.5)
```

##### Train the model

```python
model = image_classifier.create(train_data)
```

##### Test the model

```python
loss, accuracy = model.evaluate(test_data)
```

##### Export the model

```python
model.export(export_dir='.')
```
#### Run the model 
```python
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import cv2

interpreter = tf.lite.Interpreter(model_path="model.tflite")
interpreter.allocate_tensors()
output = interpreter.get_output_details()[0]  

input = interpreter.get_input_details()[0]
input_data = tf.constant(1., shape=[1, 1])

image = "image.png"
image = cv2.imread(image, cv2.IMREAD_UNCHANGED)
shape = interpreter.get_input_details()[0]['shape']
image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
image = np.asanyarray(image, dtype="uint8")
image = cv2.cvtColor(image, cv2.COLOR_GRAY2RGB) 
image = cv2.resize(image, (shape[1], shape[2])) 

plt.imshow(image)
image = image.reshape(shape)
interpreter.set_tensor(input['index'], image)
interpreter.invoke()

val = (interpreter.get_tensor(output['index'])[0])
acum = 0
max = 0
    
for i in range(len(val)):
    acum += val[i]
    if val[max] < val[i]:
        max = i

data = ['A', 'B', 'C', 'D','E', 'F', ' G', 'H','I']

print(val)
print(data[max])

```

![F](/assets/LARC/F.png) ![Stats](/assets/LARC/stats.png) 
# Seat detection

Detecting an available seat is a procedure necessary for the Receptionist Task, where there are either couches or chairs and the robot should be able to point to a position where there is a free seat, considering that there could be people already sitting on some of them.

## Detection

In order to detect people, chairs and couches, the `YOLO v8` model was used, filtering by classes.

- Person: class 0
- Chair: class 56
- Couch: class 57

```python
# Get the results from the YOLOv8 model
results = self.model(frame, verbose=False, classes=[0,56,57])

people = []
chairs = []
couches = []

for out in results:
    for box in out.boxes:
        x1, y1, x2, y2 = [round(x) for x in box.xyxy[0].tolist()]
        class_id = box.cls[0].item()
        label = self.model.names[class_id]
        bbox = (x1, y1, x2, y2)

        if class_id == 0:
            people.append({"bbox": bbox, "label": label, "class": class_id})

        elif class_id == 56:
            chairs.append({"bbox": bbox, "label": label, "class": class_id})

        elif class_id == 57:
            couches.append({"bbox": bbox, "label": label, "class": class_id})
```

## Selection

To select a seat, it was a safer option to choose an available chair, so the chair array would be traversed to see if there are any chairs that don't have people sitting on them. For this, the bounding boxes of chairs were compared with the center point of each person to detect if the center point was inside the chair bounding box, meaning the seat was taken. Additionally, since there could be people and chairs behind the living room area, the available chairs were added to a priority queue, sorting by the bbox width. Finally if the queue was not empty, the node would return the center pixel of the chair with the largest area.

```python
chair_q = queue.PriorityQueue()
for chair in chairs:
    occupied = False
    xmin = chair["bbox"][0]
    xmax = chair["bbox"][2]
    y_center_chair = (chair["bbox"][1] + chair["bbox"][3]) / 2

    # Check if there is a person sitting on the chair
    for person in people:
        center_x = (person["bbox"][0] + person["bbox"][2]) / 2
        person_y = person["bbox"][3]

        if center_x >= xmin and center_x <= xmax and person_y > y_center_chair:
            occupied = True
            break

    if not occupied:
        area = xmax - xmin
        output = (chair["bbox"][0] + chair["bbox"][2]) / 2
        chair_q.put((-1*area, output, chair["bbox"][0],chair["bbox"][1],chair["bbox"][2],chair["bbox"][3]))
        
    
if len(chairs) != 0:
    space, output, a,b,c,d = chair_q.get()
    print(space)
    return self.getAngle(output, frame.shape[1])

```

On the other hand, if there were no available chairs or the seats were all couches, a different method was used, since it was possible for a large couch to have multiple people sitting on it. In such case, the largest available space should be selected. Therefore, a zero array with size of the width of the image was created. Then, for each couch detected, the spaces with people were marked as 1, according to the bounding boxes. Finally, the array would be traversed according to the bounds of the couch (xmin and xmax), performing a sliding window to find the spaces filled with 0s, which where added to another priority queue to find the largest available space.

```python
available_spaces = queue.PriorityQueue()
for couch in couches:

    # Bounds of the couch
    couch_left = couch["bbox"][0]
    couch_right = couch["bbox"][2]

    # Create a space array
    space = np.zeros(frame.shape[1], dtype=int)

    # Fill the space with 1 if there is a person
    for person in people:
        xmin = person["bbox"][0]
        xmax = person["bbox"][2]

        space[xmin:xmax+1] = 1

    # Set bounds pixels to 0
    left = couch_left
    space[couch_left] = 0
    space[couch_right] = 0

    # Traverse the space array to find empty spaces
    for i in range(couch_left, couch_right):
        if space[i] == 0:
            if left is None:
                left = i

        else:
            if left is not None:
                available_spaces.put((-1*(i - left), left, i))
                left = None


    if left is not None:
        available_spaces.put((-1*(couch_right - left), left, couch_right))

print(f"Found {len(couches)} couches")

# Get largest space, return center:
if available_spaces.qsize() > 0:
    max_space, left, right = available_spaces.get()
    output = (left + right) / 2
    print("Space found", output)
    return self.getAngle(output, frame.shape[1])

else:
    print("No couch or chair found")
    return -1
```

# Overview 

The area of speech have had different implementations using different and software solutions.
Mainly the area is composed by the two components: 

- [Speech To Text](speech_to_text.md)
- [Text To Speech](text_to_speech.md)

## Important topics
You can run all the nodes in yor machine or in the robot. The simplest usage of speech area is with three topics:

###  InputAudioActive 

Used as a flag to start listening to the user. It is a Bool topic, when it is True the robot is listening to the user, when it is False the robot is not listening to the user. Commonly used as:
    ```bash
    self.speech_enable = rospy.Publisher("inputAudioActive", Bool, queue_size=10)
    ```

### Saying

Used to know the status of the robot, when it is talking or not. It is a Bool topic, when it is True the robot is talking, when it is False the robot is not talking. Commonly used as:
    ```python
    self.say_listener = rospy.Subscriber("saying", Bool, self.say_callback)
    ```

### RobotText

Used to send the text that the robot will say. It is a String topic, when it is published the robot will say the text. Commonly used as:
    ```python
    self.robot_text = rospy.Publisher("robot_text", String, queue_size=10)
    ``` 

The regular function of a say action is the following:

```python
def say(self, text):
    time.sleep(1)   
    if self.saying == False:
        self.saying == True
        self.say_publisher.publish(text)
    time.sleep(len(text)/8 )    
``` 
That becasue of the natural delay of the robot to say the text depending on the length of the text.

### RawInput

Is the processed text that the robot understood from the user. It is a String topic, when it is published the robot will say the text. Commonly used as:
    ```python
    self.raw_input_listener = rospy.Subscriber("rawInput", String, self.raw_input_callback)
    ```

With a simple usage of the speech area you can use the following code to get the text from the user:

```python
def input_callback(self, msg):
    self.inputText = msg.inputText
    rospy.logdebug("I heard: " + self.inputText)
```

## Usage 

This are a basic example of implementation in python of the speech area.

### Promts

Promts are simply a dictionary of text that the robot can say. It is a dictionary of String topic, when it is published the robot will say the text. Commonly used as:
```python
promts = {
"start": "Let's start General Purpose Service Robot ",
"wait": "I am waiting for someone",
"self_intro": "Hello, I'm homie, your personal assistant.",
"ask_comm": "What do you want me to do ?",
"ask_drink": "What is your favorite drink?",
"repeat": "i think i didn't get that, can you repeat it?",
"come" : "come with me ",
"unreached": "I am sorry, I couldn't understand your command, pleasy try again ",
"sorry" : "I am sorry, I think i didn't get that",
"assign": "Can you please sit on the chair in front of me?"
}
```
they are used as:
```python
self.say(promts["start"])
```

### Calls 

Calls are for GPT-3 API, they are simply a dictionary with instructions to get the entitie of a text
```python
calls = {
"get_name": "if there is nos name return ''get me the name of the person on the next message: ",
"get_drink": "get me the name of the beverage on the next message: ",
"describe": "describe a person with the next attributes: ",
"confirm" : "tell me onlyTrue or False, the next message is a general confirmation, like ues, OK, got it:",
"reached": "tell me only True or False, the next message is a confirmation that you reached a place:",

"get_loc": "the valid locations are living room, dining room, kitchen, bedroom. Give me only the location on this message: ",
"get_obj": "the valid objects are coke, apple, mug, soap, banana, pitcher. Now give me only the object on this message: ",
"get_per": " the valid names are Jamie, Morgan, Micheal, Jordam, Taylor, Tracy, Robin, Alex. Now give me only the name on this mesage: "
}
```

commonly used as:
```python
intent = self.parser(calls["describe"] + " " + str(self.persons[0].name) + " " + str(self.persons[0].race) + " " + str(self.persons[0].age))
self.say(intent)
```

## Configuration

As the system requires hardware input and output you need to be aware of some details.

### Microphone

Select the correct microphone in your system, the easiest way is by graphical interface, but you can also do it by command line. 

```bash
arecord -l
```
You will get something like this:
```bash
**** List of CAPTURE Hardware Devices ****
card 0: PCH [HDA Intel PCH], device 0: ALC295 Analog [ALC295 Analog]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
```
Then you plug the microphone and run the command again:
```bash
**** List of CAPTURE Hardware Devices ****
card 0: PCH [HDA Intel PCH], device 0: ALC295 Analog [ALC295 Analog]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 2: Device [USB PnP Sound Device], device 0: USB Audio [USB Audio]
  Subdevices: 0/1
  Subdevice #0: subdevice #0
```
so you can see that the microphone is in the card 2, device 0. So you can set the environment variable:
```bash
export ALSA_INPUT_DEVICE=hw:2,0
```
That should be enough to make the microphone work, but you can record a test to check it:
```bash
arecord -f cd -Dhw:2,0 -d 10 test.wav
```
You can play the test:
```bash
aplay test.wav
```

### Speaker

You have two options to configure the speaker, you can: 

- Use a bluetooth speaker and run the say node in your computer, so you can use the bluetooth speaker with your computer.
- Use the speaker connected to a plug in the robot and run the say node in the robot. You need to configure the output interface in the jetson, the easiest way is by graphical interface, but you can also do it by command line. 

You should do the same as with the microphone, but with the output device:
```bash
aplay -l
```
You will get something like this:
```bash
**** List of PLAYBACK Hardware Devices ****
card 0: PCH [HDA Intel PCH], device 0: ALC295 Analog [ALC295 Analog]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: Device [USB PnP Sound Device], device 0: USB Audio [USB Audio]
    Subdevices: 1/1
    Subdevice #0: subdevice #0
    ```
Then you plug the speaker and run the command again:
```bash
**** List of PLAYBACK Hardware Devices ****
card 0: PCH [HDA Intel PCH], device 0: ALC295 Analog [ALC295 Analog]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 1: Device [USB PnP Sound Device], device 0: USB Audio [USB Audio]
    Subdevices: 1/1
    Subdevice #0: subdevice #0
card 2: Device_1 [USB PnP Sound Device], device 0: USB Audio [USB Audio]
    Subdevices: 1/1
    Subdevice #0: subdevice #0
```
so you can see that the speaker is in the card 2, device 0. So you can set the environment variable:
```bash
export ALSA_OUTPUT_DEVICE=hw:2,0
```
That should be enough to make the speaker work, but you can play a test to check it:
```bash
aplay -test.wav
```


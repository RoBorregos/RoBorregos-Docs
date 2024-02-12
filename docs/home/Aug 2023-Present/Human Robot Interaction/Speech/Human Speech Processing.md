# Human Speech Processing
One aspect that enhances the human-robot interaction is the robot’s ability to interpret human speech and generate text which can be processed further down the pipeline. Several approaches can be used to interpret speech and produce the desired outcomes, each involving trade-offs between precision and computational resources used. The approach followed to achieve a robust implementation consisted of dividing the relevant steps among nodes that handle each task. This promotes decoupling, which not only allows for the replacement of a specific node (for potential improvements) without affecting the other ones, but it also facilitates the usage of the produced code in other projects. In addition, it allows for a highly configurable system, which allows us, for instance, to choose between a node with high precision and computational resources and a node with regular precision with reduced system overhead.

The main nodes in the pipeline consist of Voice Activity Detection (VAD) and Speech To Text (STT). 

## **Voice Activity Detection**

The VAD module is used to filter the input data given to the STT node. This reduces the overall computational resources used as the VAD requires less resources than the Speech to Text node. To recognize voice activity, Silero VAD, a pre-trained voice activity detector was used. However, the node can also be configured to use webrtcvad, which is slightly faster than Silero VAD but has a reduced precision for our use case. Both options are used to classify audio segments as voiced or unvoiced, which are then collected and sent to the next node in the pipeline if they fit the established parameters of duration and speech probability. 


## **Speech to Text**

For the Speech to Text node, Whisper was used: Whisper is a general-purpose speech recognition model, which can perform Multilingual speech recognition, speech translation, and language identification. Whisper can be used with several models that vary in precision and computational resources, meaning that it can be suitable in a diverse set of situations. However, the system can be configured to use Speech Service from Microsoft Azure as in previous years of development, in case there are low computational resources available.

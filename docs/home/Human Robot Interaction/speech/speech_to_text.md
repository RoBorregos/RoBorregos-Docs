# Speech To text

The speech to text area is composed by the following ROS nodes:

1. AudioCapturer

    **devices/AudioCapturer [python]**: It is a node that captures the audio using [PyAudio](https://pypi.org/project/PyAudio/) and publishes it to the topic _rawAudioChunk_.

2. GetUsefulAudio
    
    **devices/UsefulAudio [python]**: A node that takes the chunks of audio and, using [webrtcvad](https://github.com/wiseman/py-webrtcvad), checks for a voice, cut the audio and publishes it to the topic _UsefulAudio_.
    Webrtcvad approach was made as an alternative that don´t remove silence but obtains the pieces of audio when someone speaks perfectly, it has a very good performance.

3. Engine Selector 
    
    **action_selectors/hear [python]**: This node receives the requests of STT. It checks if there is an internet connection, to know whether to call the offline or online engine; this can be overridden with FORCE_ENGINE parameter.
    * Online engine: it is in _AzureSpeechToText_ node. For that, this node processes the audio of _UsefulAudio_ do a resample of 16KHz and publishes it in a new topic called _UsefulAudioAzure_ to relay it to that node. 
    * Offline engine: it is in _DeepSpeech_ node. For that, this node redirect the audio of _UsefulAudio_ to a new topic called _UsefulAudioDeepSpeech_ to relay it to that node.

4. Speech to text Engines
    Both are nodes that receive the audio and convert it to text. The difference is that one is online and the other is offline. 
    
    **action_selectors/AzureSpeechToText [c++]**: A node that takes the audio published in the topic _UsefulAudioAzure_ and send it to the [Azure SpeechToText API](https://azure.microsoft.com/es-mx/services/cognitive-services/speech-to-text/), receives the text and publishes it to the topic _RawInput_.
    
    **action_selectors/DeepSpeech [python]**: A node that takes the audio published in the topic _UsefulAudioDeepSpeech_ and calls [DeepSpeech2](https://arxiv.org/abs/1512.02595), converts it to text, and publishes it to the topic _RawInput_.

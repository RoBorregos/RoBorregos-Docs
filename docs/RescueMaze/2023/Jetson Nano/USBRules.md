# USB Port automatization
# This file is part of the RoBorregos RescueMaze project.

Here is an example of how we can se the behaviour of the usb ports.

![USB Ports Behaviour](/assets/maze/ports_behaviour.jpeg)

## Udev Rules

For the USB port automatization, we use udev rules. These rules are located in the `rules.d` folder. The rules are loaded by the udev daemon when the system starts. The udev daemon monitors the kernel for events and executes the rules when a device is added or removed.

To automate the USB ports, we need to create a rule for each port. The rule will be executed when the port is connected to the computer. The rule will execute a script that will set the port to the desired mode.

Here is the hard investigation we did to determine the rules:

![example1](/assets/maze/chat_gpt0.jpeg)

![example2](/assets/maze/chat_gpt1.jpeg)


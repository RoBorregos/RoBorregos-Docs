# State Machine

A state machine is a software design pattern that allows for a consistent and predictable behaviour of the robot. It is a state-based approach to programming, where the robot is in a certain state at a given time, and it can transition to different states based on the inputs and outputs. This paradigm allows for performing robot control in an imperative way, focusing on the what rather than the how of the robot's behaviour.

The robot had its own state machine which were mostly tied to different part of the game mechanics and gameplay, i.e picking objects, moving, etc. 

The update() method of the robot handled state machine transitions.

## Subsytem state machines

To ensure consistency and predictability of the robot, each subsytem had its own state machine which controlled the subsytem's current action and behaviour. The control of the state machine was granted through the setState() method of the subsystem, provided via the common subsystem interface.

This way, every subsystem was tied to a set of finite states, which could be controlled externally by the main robot state machine, and assured that each subsystem would perform its task independently of the other subsystems. This approach allowed for a modular and flexible design of the robot, were specific mechanism behaviours could be simply added or removed to each subsytem without affecting the rest of the robot.

Note: The setState() method received an integer value, which represented the state of the subsytem. Each subsytem had its own set of states, which were defined in an enum within the subsystem's implementation.


## Main robot state machine

After each subsystem had its own state machine, the main robot state machine was responsible for coordinating the actions of all the subsystems. This state machine was responsible for the overall control of the robot, including the game mechanics and gameplay. The behaviour of the main robot state machine is very similar to the subsystem state machines, mimicking the same structure and logic but at a higher level of abstraction.

Each robot task was tied to a specific state, with end conditions for managing the transition to the next state based on the information present. 

Within each state, a smaller, simpler state machine was implemented to handle specific, consecutive or dynamic actions. This was done via the following variables:
```cpp
action_stage
action_start_time
```

The action stage was the current action (sub-state) of the current state. The action start time was the time when the action started, and was used to track the duration of the action. For example, if a state had 3 actions, the action stage would be 1, 2, or 3, and the action start time would be the time when each action started. 

This approach allowed for controlling the state via specific sub-actions that could be added or removed as needed, and for specific conditions, such as sensor info or timing conditions, to manage the transition to the next action/state.

Each state would handle its own actions (sub-states), action_stage and action_start_time via direct modifications of the variables. Whenever the state was finished, the state would handle the setState() of the main state machine to transition to the next state, which would be handled by the update() method of the main state machine in the next loop.

Note: to eensure that the action_stage and action_start_time variables were updated correctly, aside from direct modifications made within each state, whenever the setState() of the main state machine method was called, these were also reset to 0. 
# Kicker and Dribbler

## 1. Implementation
The opportunity to have a kicker and a dribbler is an excellent alternative for strategy and also contributes significantly during competition. In our year, these were not implemented, but their integration is highly recommended for future versions.

## 2. Recommendations

* **Board Separation:** Although the current design integrates these two components into the Main Board, it is strongly recommended to implement them on a **separate board**.
* **Electronic Noise:** This separation is mainly due to the electronic noise and the significant electrical problems that the kicker's operation can produce, which could interfere with the microcontroller.
* **Space Optimization:** Regarding the dribbler, moving its implementation to a board separate from the Main Board would be beneficial to free up physical space on the main PCB, allowing for the addition of extra components or better trace routing in the future.
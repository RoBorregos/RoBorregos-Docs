# 🧭 STM32H7 Omnibase ROS2 Interface

This repository contains the ROS 2 nodes and documentation for interfacing an STM32H7-based omnidirectional robot base with ROS 2 via serial communication. 

---

## 📚 Contents

- [Status](#-status)
- [To Do](#-to-do)
- [Topic Overview](#-topic-overview)
- [ros2 development (home_electronics branch)](#-ros2-development-home-electronics-branch)
- [STM32 Project Repository](#-stm32-project-repository)
- [Appendix](#appendix)

---

## 📢 Status
Control for one motor succesfully tested, must check PID gains & test 4 motors simultaneously

---

## 📌 Current Pinout

| PIN  | Función                                    |
|------|--------------------------------------------|
| PB6  | BNO055: I2C_SCL                             |
| PB7  | BNO055: I2C_SDA                             |
| PD4  | H Bridge 1_IN1 (GPIO OUTPUT PP)            |
| PD5  | H Bridge 1_IN2 (GPIO OUTPUT PP)            |
| PD6  | H Bridge 1_IN3 (GPIO OUTPUT PP)            |
| PD7  | H Bridge 1_IN4 (GPIO OUTPUT PP)            |
| PE2  | H Bridge 2_IN1 (GPIO OUTPUT PP)            |
| PE4  | H Bridge 2_IN2 (GPIO OUTPUT PP)            |
| PE3  | H Bridge 2_IN3 (GPIO OUTPUT PP)            |
| PE6  | H Bridge 2_IN4 (GPIO OUTPUT PP)            |
| PA0  | H Bridge 1_PWM 1 (ENA TIM5 CH1)            |
| PE5  | H Bridge 1_PWM 2 (ENB TIM12 CH1)           |
| PB14 | H Bridge 2_PWM 1 (ENA TIM14 CH1)           |
| PF9  | H Bridge 2_PWM 2 (ENB TIM15 CH1)           |
| PE9  | Encoder 1 Signal A (TIM1 CH1)              |
| PE11 | Encoder 1 Signal B (TIM1 CH2)              |
| PA5  | Encoder 2 Signal A (TIM2 CH1)              |
| PB3  | Encoder 2 Signal B (TIM2 CH2)              |
| PD12 | Encoder 3 Signal A (TIM4 CH1)              |
| PD13 | Encoder 3 Signal B (TIM4 CH2)              |
| PC6  | Encoder 4 Signal A (TIM8 CH1)              |
| PC7  | Encoder 4 Signal B (TIM8 CH2)              |

---

## 📡 Current Topic Overview

| Topic Name                | Msg Type             | Description                                                                  |
|--------------------------|----------------------|-------------------------------------------------------------------------------|
| `/stm32_debug`           | `std_msgs/String`    | Full parsed received data                                                     |
| `/stm32/pose`            | `Float32MultiArray`  | Desired pose and distance from robot center, radius received by stm32 `[x, y, phi, d, r]`|
| `/stm32/imu`             | `Float32MultiArray`  | IMU angles `[roll, pitch, yaw]`                                               |
| `/stm32/omegas`          | `Float32MultiArray`  | Wheel angular speeds (calculated with encoders)`[ω1, ω2, ω3, ω4]`             |
| `/stm32/real_speeds`     | `Float32MultiArray`  | `[phi_dot, x_dot, y_dot]` Forward kinematics calculation of inertial speeds   |
| `/stm32/odom`            | `Float32MultiArray`  | Odometry: `[phi, x, y]` <-- gotten by integrating                             |
| `/stm32/errors`          | `Float32MultiArray`  | Pose error: `[dx, dy, dphi]` current pose error                               |
| `/stm32/ctrl_speeds`     | `Float32MultiArray`  | Local velocities control output `[x_dot, y_dot, phi_dot]`                     |
| `/stm32/ctrl_u`          | `Float32MultiArray`  | IK calculated necessary wheel speeds to get local ctrl velocities `[u1, u2, u3, u4]`|
| `/stm32/pwm`             | `Int32MultiArray`    | Motor PWM duty cycles necessary to achieve ctrl ik u1-4 `[d1, d2, d3, d4]`    |
| `/stm32/timing`          | `Int32MultiArray`    | Timing `[ts_now, ts_prev, ts_delta]`                                          |
| `/stm32/encoders`        | `Int32MultiArray`    | Encoder CNT register (current counter value)`[TIM1, TIM2, TIM4, TIM8]`        |
| `/stm32/pid_gains/x`     | `Float32MultiArray`  | X position PID gains                             |
| `/stm32/pid_gains/y`     | `Float32MultiArray`  | Y position PID gains                             |
| `/stm32/pid_gains/phi`   | `Float32MultiArray`  | Heading PID gains                                |
| `/stm32/pid_gains/u0`    | `Float32MultiArray`  | Motor 1 PID gains                                |
| `/stm32/pid_gains/u1`    | `Float32MultiArray`  | Motor 2 PID gains                                |
| `/stm32/pid_gains/u2`    | `Float32MultiArray`  | Motor 3 PID gains                                |
| `/stm32/pid_gains/u3`    | `Float32MultiArray`  | Motor 4 PID gains                                |

---

## 🔗 ROS2 Development home electronics branch

👉 [GitHub Repo for ROS2 development so far](https://github.com/RoBorregos/home-electronics/tree/omnibase)

---

## 🔗 STM32 Project Repository

👉 [GitHub Repo for STM32ubeIDE Project **STM32H7_OMNIBASE_FREERTOS**](https://github.com/Roger412/STM32H7_OMNIBASE_FREERTOS.git)

---

## 📎 Appendix

- **Requirements document**: [Link](https://tecmx-my.sharepoint.com/personal/a00833121_tec_mx/_layouts/15/Doc.aspx?sourcedoc={4dfd3cbd-9f97-41d5-bf75-bfc26095b554}&action=edit&wd=target%28Desarrollo%20de%20placas%202025%20%40Home.one%7Cfed71162-acfd-4908-89ff-7ce12ce7cbc1%2FPlaca%20de%20control%7Cbf95b886-4d32-43c5-ad00-1bbc99f5f7f8%2F%29&wdorigin=NavigationUrl)

### 🕘 Revision History

| Version | Date       | Changes Made           |
|---------|------------|------------------------|
| 1.0     | 2025-05-24 | Initial release        |


1. [STM32H755 Documentation & Datasheets](https://www.st.com/en/evaluation-tools/nucleo-h755zi-q.html#documentation)
2. [ODrive S1 Datasheet](https://docs.odriverobotics.com/v/latest/hardware/s1-datasheet.html#pinout)

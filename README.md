# 4-Wheel Wireless Joystick-Controlled Rover

## Overview

This project is a **4-wheel wireless joystick-controlled rover** developed using **Arduino UNO and Arduino Mega 2560**.

The rover has **four wheels in total**, consisting of **two motor-driven wheels** and **two free/support wheels**. The two motor-driven wheels provide the movement of the rover, while the two free wheels provide support and stability.

The **Arduino UNO acts as the transmitter**, while the **Arduino Mega 2560 acts as the receiver**. A joystick connected to the Arduino UNO is used to control the rover's movement.

The movement commands are transmitted wirelessly from the Arduino UNO to the Arduino Mega 2560 using **NRF24L01 wireless communication modules**. The Arduino Mega receives the commands and controls the two motor-driven wheels through **BTS7960 (IBT-2) motor drivers**.

An **I2C OLED display** is connected to the Arduino Mega to display the current movement status. A communication timeout mechanism is also implemented to automatically stop the motors when wireless communication is lost.

---

## Key Features

- 4-wheel rover configuration
- 2 motor-driven wheels
- 2 free/support wheels
- Wireless joystick control
- Arduino UNO-based transmitter
- Arduino Mega 2560-based receiver
- NRF24L01 wireless communication
- BTS7960 (IBT-2) motor drivers
- Forward, Backward, Left, Right and Stop control
- I2C OLED movement/status display
- Communication-loss safety stop

---

## Hardware Components

| Component | Quantity |
|---|---:|
| Arduino UNO | 1 |
| Arduino Mega 2560 | 1 |
| NRF24L01 Wireless Module | 2 |
| Joystick Module | 1 |
| BTS7960 (IBT-2) Motor Driver | 2 |
| DC Geared Motor | 2 |
|  Wheels | 4 |
| I2C OLED Display | 1 |
| 7.4V Battery | 1 |
| Rover Chassis | 1 |
| Connecting Wires | As required |

---

## Wheel Configuration

The rover has **4 wheels in total**. Initially, all four wheels were connected to DC geared motors.

During testing, it was observed that the available battery supply was not sufficient to reliably power all four motors simultaneously. Therefore, the rover was tested and operated using **2 motor-driven wheels**, while the remaining **2 wheels were used as free/support wheels**.

The two motor-driven wheels are controlled independently using two BTS7960 (IBT-2) motor drivers.

- 4 wheels are physically present on the rover.
- 2 wheels are motor-driven during the final implementation.
- 2 wheels act as free/support wheels.
- The free/support wheels provide stability and rotate freely with the movement of the rover.
---

# System Architecture

```text
                    JOYSTICK
                       │
                       ▼
               ┌─────────────────┐
               │   Arduino UNO   │
               │   TRANSMITTER   │
               └────────┬────────┘
                        │
                        ▼
                  ┌───────────┐
                  │ NRF24L01  │
                  └─────┬─────┘
                        │
               WIRELESS COMMUNICATION
                        │
                        ▼
                  ┌───────────┐
                  │ NRF24L01  │
                  └─────┬─────┘
                        │
                        ▼
               ┌─────────────────┐
               │ Arduino Mega    │
               │    RECEIVER     │
               └───────┬─────────┘
                       │
                ┌──────┴──────┐
                │             │
                ▼             ▼
        ┌─────────────┐ ┌─────────────┐
        │  Left IBT-2 │ │ Right IBT-2 │
        └──────┬──────┘ └──────┬──────┘
               │               │
               ▼               ▼
        Left Powered      Right Powered
           Wheel             Wheel

                       │
                       ▼
                 OLED DISPLAY
                Movement Status

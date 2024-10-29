# Common Firmware
Low-level firmware reused as a dependency in my projects.

## Background
Microcontrollers are affordable tools for implementing software in real-time, but each manufacturer has their own tool-chain. After graduating from engineering school, I went to my Cortex-M-based dev board. I wanted to develop microcontroller software in my free time to continue learning and to implement my ideas just for the sake of my creativity.

To my dismay, I found I didn't have the version of the manufacturer's IDE that supported that board and could no longer download the old tools needed to program the board. Later, I found that the CMSIS peripheral HAL that I learned for that manufacturer did not match what other manufacturers of Cortex-M microcontrollers use (each is different).

This project seeks to create a common low-level firmware that is not tied to any particular manufacturer. It uses Make and the Arm GNU compiler so is also portable between IDE's. Lower levels are written in C and some upper level drivers are written in C++ to benefit from object-oriented programming constructs.

## Architecture Diagram
![Common Firmware Architecture](/_doc/CommonFirmwareArch.PNG)

## Architecture Description
Goals:
1. Code which is dedicated to a particular manufacturer's chip is contained to a small section of the project
2. Drivers for other chips (ex. sensors) are separated so they do not directly use the microcontroller's HAL
3. A clear hierarchy which allows for tracing upper level functions to lower level microcontroller HAL access
4. Utilize CMSIS HAL for microcontroller register access for clarity and for comparisons with examples in the literature

L1_HW_HAL is simply the CMSIS HAL for the core and the manufacturer's peripherals without modification.

L2_System_Config level is the only portion to modify when switching to another microcontroller with the same manufacturer.

L3_Driver_Low is the only additional portion to modify when switching to another manufacturer's microcontroller.

L4_Driver_High is drivers for other chips (ex. sensors), NewLib support files and intermediary classes that the higher-level app code can call.

EnviRobo
========================
Usage: ./userInput [FILE_NAME]

The purpose of this project is to build an autonomous robot using a TI MSP432 board and the
Raspberry Pi that is able to trap certain objects and move them to a desired location. Input data
from digital distance sensors around the robot will help it navigate its environment, and a camera
takes pictures for object recognition. The robot’s current actions are displayed on an OLED
attached in the back, along with a compass to help with navigation and location. Servo motors
are used to control the motion of the robot and the trapping mechanism. The two boards
communicate through a UART connection, with the MSP432 asking for object recognition
compass data from the Raspberry Pi. This device is designed to work within a defined
environment for specific object recognition. 

A Bluetooth Module RN42 BlueSMIRF is added for user to be able 
to switch modes from autonomous to manual and vice versa. A MATLAB GUI is developed
to control the communication between EnviRobo and the user on a PC.

Top Level Diagram
-------------------------------
![Alt text](https://github.com/map34/EnviRobo/blob/master/TopLevelDiagram.png "Figure 1: Top Level Diagram of EnviRobo")

Code Reference
-------------------------------
● compass.py is used for running the image capturing/ processing code and compass
sensors that switch between Python 3 and Python 2 back and forth
● msp_rasp_1.py is used for the actual object recognition as well as sending flags through
UART to MSP432
● main.c is used for running the robot control logic used on the MSP432 board

## Rapid Rescuer
This repository contains code for Rapid Rescuer Bot using Atmega2560 and ESP8266 built for e-Yantra Robotics Challenge 2019-20.
### Theme 
> Rescue operations pose a grave danger to victim as well as to those assigned the job of saving the lives of victims. Rescuing people in case of fire and explosion in any city-scape comprising numerous buildings is a risky and crucial operation. 
Robots can be used in such applications to save lives of people and also provide crucial information that can further be used by experts for decision making. The purpose of Rapid Rescuer is to rescue victims from dangerous or unpleasant situations to safer locations. 

###  Methodology
* A city is abstracted in form of a maze. 
* Using image processing shortest path from start to end node is calculated using Dijkstra's Algorithm.
* Coordinate information is communicated to ESP32 using socket programming. 
* Using UART communication all useful data received by ESP32 is fed to Atmega2560.
* Robot gets fire-zone coordinates,number of people to be evacuated ,path to be followed and vacancies in each hospital from base station.
 * Sensed values from  IR sensor array and 3 ToF sensors are fed to micro-controller running code which makes it to avoid collisions with walls ,transverse the maze and rescue people from fire-zone and take them to hospital. 
### Bot Design and Arena

<img src="https://github.com/amrathesh/Rapid-Rescuer/blob/master/bot_v1.jpeg" width="400" height="650"/> <img src="https://github.com/amrathesh/Rapid-Rescuer/blob/master/dark_lit.jpg" width="400" height="650"/> 
<img src="https://github.com/amrathesh/Rapid-Rescuer/blob/master/arena_maze.jpg" width="930" height="600"/>



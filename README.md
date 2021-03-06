<h4 align="center">
<img src="https://github.com/danilogoan/AquarismSupportSystem/blob/main/Qt/AquarismSupportSystem/Images/pngLogo.jpg" width="200"><br>
	<img alt="APM" src="https://img.shields.io/apm/l/vim-mode"><br>
        🐟🐡 Project Finished... Some improvements may happen 🐡🐟
</h4>

<p align="center">
 <a href="#-Aquarism-Support-System">Aquarism Support System</a> • 
 <a href="#Features">Features</a> • 
 <a href="#Interface">Interface</a> • 
 <a href="#Aquarium">Aquarium</a> • 
 <a href="#Prerequisites">Prerequisites</a> • 
 <a href="#Group">Groups</a>
</p>

# 🐠 Aquarism Support System
This is a project for the Data Acquisition and Interface Systems subject.\
The main objective is to create a system to support an aquarium. That way, it'll be possible to monitor and control the temperature, indicate whether it is the time to clean the aquarium and feed the fish or not.\
The main dispositive is an arduino connected to an interface created with Qt Creator.

## Features
- [x] Monitor Temperature
- [x] Get real time
- [x] Add LEDs (red indicates that the temperature is not within the standards and the yellow indicates the cleanning time)
- [x] Add Buzzer (indicates the feeding time)
- [x] Simulate
- [x] Create interface
  - [x] Communicate with Arduino
  - [x] Set max and min Temperature
  - [x] Set temperature alert
  - [x] Get feeding time
  - [x] Get cleaning time
  - [x] Set feed and cleaning periods
  - [x] Get next feeding time
  - [x] Get next cleaning time
  - [x] Set feeding alert
  - [x] Set cleaning alert
  - [x] Clean button
  - [x] Feed button
  - [x] Implement bidirectional serial communication
  - [x] Implement temperature control
  - [ ] Improve temperature control
- [x] Add first Peltier module, Cooler and Relay (to cool)
- [ ] Add second Peltier module, Cooler and Relay (to heat)
- [ ] Automatic feeding
- [ ] Automatic cleaning
- [ ] Oxigen saturation
- [ ] Get real caracterization of the temperature variation using the peltier module
- [ ] Get the PI controler after the caracterization (SIMC)

## Interface
![alt text](https://github.com/danilogoan/AquarismSupportSystem/blob/main/Qt/AquarismSupportSystem/Images/Interface.jpg?raw=true)

## Aquarium
![alt text](https://github.com/danilogoan/AquarismSupportSystem/blob/main/Qt/AquarismSupportSystem/Images/Aquarium.jpeg?raw=true)

[Video of the Aquarism Support System Working](https://drive.google.com/file/d/1UjRD1MoRqijbCDu60xRZ3RDBGbESrOMo/view?usp=sharing)

## Prerequisites
The Arduino Uno was used with the devices explained on the [slide](https://github.com/danilogoan/AquarismSupportSystem/blob/main/SADI_Aquarism_Support_System_Slides.pdf), where is also explained how to execute and use the interface.
Besides that, the interface was generated and tested on Windows 10 x86.

- Clone the [repository](https://github.com/danilogoan/AquarismSupportSystem/)
- Load the [Arduino file](https://github.com/danilogoan/AquarismSupportSystem/blob/main/Arduino/AquarismSuportSystem/ArduinoAquarism.ino) into the Arduino Uno
- Unzip the [Executable](https://github.com/danilogoan/AquarismSupportSystem/blob/main/AquarismSupportSystemExe.zip)
- Open the AquarismSuportSystem.exe

You can also read the detailed [documentation](https://github.com/danilogoan/AquarismSupportSystem/blob/main/SADI_Aquarism_Support_System_Documentation.pdf).

## Group:

- Danilo Gomes de Andrade
- Henrique Dantas Silva
- João Victor Rodrigues Guimarães
- João Victor Rodrigues Ramalho
- Marley Lobão de Sousa

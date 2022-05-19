# LAMP MESH NETWORK (NLT2112)
#### The repo contains all the firmware codes for the project and instructions on how to load them to the hardware
<img src="https://user-images.githubusercontent.com/20322653/169380834-74515872-feed-4530-92fb-4ae0f10ac75c.png">

## Requirements
1) Set up the IDE, for this application you will need to download the Arduino ide [here](https://www.arduino.cc/en/software) if you don't have it already installed for your distributions
2) Instal the board support package for ESP32 by goint to ` Tools > Board:xx > Board Manager`. In the search-bar type ESP32 and select install ESP32 by Espressid Systems.  
3) Now select DOIT ESP32 DEVKITT V1 from ` Tools > Board:xx > ESP32 Arduino > DOIT ESP32 DEVKITT`
Once done proceed to the next instructions.
## Download the files 
4)  Download all the files on the `Code`:arrow_down_small: on this platform, In your downloads you will get the a folder :file_folder:  `lamp_node_firmware`this will have:
     - lamp_node_firmware.ino
     - variables.h
</br> and `Libraries` zip file -this contains all the related libraries used in the project

 ## Library Installation
5)  On library installation unzip the folder :file_folder: `libraries` in the downloaded folder :file_folder:, copy and paste all the folders to your directory `documents/arduino/library` folder :file_folder:
6)  Open the `lamp_node_firmware`  in `lamp_node_firmware.ino`folder :file_folder: with the downloaded ide, compile and upload to the hardware `(procedure on how to use the ide is on the tutorial link)`:link:    [tutorial](https://www.youtube.com/watch?v=nL34zDTPkcs&t=3s)

# Uploading the codes  :arrow_up:
You can buy the FTDI from here :link: [buy ftdi](https://www.amazon.com/HiLetgo-FT232RL-Converter-Adapter-Breakout/dp/B00IJXZQ7C/ref=sr_1_3?keywords=FTDI&qid=1650483928&sr=8-3)
or here 
 :link: [buy other FTDI](https://www.amazon.com/CP2102-Module-Converter-Downloader-Compatible/dp/B092YMT52G_)

1) Insert the FTDI programmer cable socket onto UART headers. ...
2) <img src="https://github.com/skndungu/compressor_controller/blob/main/assets/img/USB-TTL-Connection.jpeg" height="200">
3) Open Arduino IDE software. ...
4) Next, call out the source code. ...
5) Select “FTDI” from the Tools > Programmer menu.
6) Select “Upload using Programmer” from the File menu.

You can also explore more from the following tutorial :link:[How to Use FTDI Programmer to upload Code](https://www.youtube.com/watch?v=JYchUapoqzc)
## Debugging 
To see the serial data as sent or received from either of the boards we can use the FTDI programmer or any USB to TTL converter so that you can have access to a serial port. Check out the headers on the board for uart communications 

## LAMP MESH NETWORK
The lamp mesh is made at its core with ESP32. this IC contains a WiFi and Bluetooth(BLE) PHY that enables radio communication. 

MESH is a networking protocol built on top of wifi which allows for interconnectiong of numerous devices spread over a vast area to a single network
This means that data made available to the network can be accessed by another device or node in the same network. 
The coverage of a mesh netwrk is also large and a single network can hold upto 1000 devices with ease.
For a mesh network to be function using esp32, then, the device wishing to connect to the network must have the same MESHNAME and MESHPASSWORD. 
Each device on the mesh is however, assigned a special ID upon joining.

In order to keep track of the nodes and orientation in which they are, we also assign each device(or node) a sequential ID number stored in the variable `myNodeID`

### NOTE :
`variables.h` file contains the `MESHNAME`, `MESHPASSWORD` and `myNodeID` variables which can be customized according to the user needs. 
in order for a device to join a network, `MESHNAME` and `MESHPASSWORD` must be the same as that of other devices in the mesh network.
However, the `myNodeID` must be uniques and arranged sequentially as this will be important in the code implemetation.
i.e lamp1 (myNodeID = 1), lamp2(myNodeID = 2).... and so on.

##  Working Principle
The device, once powered and it successfuly connects to the mesh network, it can start monitoring various states.
#### state 1: 
If the device motion sensor is triggered(detects an object) a signal is sent to the device and thus the device turns on the lamp and sends its ID (i.e `myNodeID`) and an "ON"status to the network
Other devices on the network pick this information and deoes calculation and comparisons 
</br>
#### state 2: 
When the device is not triggered by the motion sensor(there is no motion detected) it listens for data flowing in the mesh  network. once data is available in the network the device picks it and confirms its state(ON) and ID.
Then it computes the position of its ID with respect to the ID received and compares it. If it finds that its ID is within the desired range, then it turns on. If not is remains off until a device near it is triggered. This is repeated and thus data and status is always upto date

The range is specified in the variable.h file. It can either be `#define five_lamps 0` - meaning not in five lamp range mode or `#define five_lamps 1` - maning five lamp range mode is active.

The device(s) can be debugged by connecting it to a computer and retriving the necessary information via the serial monitor.
 



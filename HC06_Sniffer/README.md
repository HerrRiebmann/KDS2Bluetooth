#HC06 Sniffer

A little Sketch to receive the following requests from different Apps:
* AT-Commands
* PID-Lists
* PID´s

The responses are static values. Unknown Commands will be submitted to the user by Serial.write.

This sketch was a workaround to get to know which commands where asked by my Virb XE Action Cam and different OBD II-Apps.
All results from my investiagtions where included into the [ECU Reader](https://github.com/HerrRiebmann/KDS2Bluetooth/tree/master/ECU_Reader)-Sketch.

###Prequisites
* Arduino + Bluetooth module
* Arduino connected to PC via USB
* Serial Monitor
* App (*Smartphone, Tablet, PC, ect*) to read OBD II Data

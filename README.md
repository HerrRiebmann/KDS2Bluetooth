# KDS2Bluetooth
Kawasaki Diagnostic Service Reader via Bluetooth

Using an Arduino (*Nano*) to read parameter from a Kawasaki motorcycle by it´s diagnostic system.
Requests can be submittet via bluetooth, by any ELM327 compatible device/application. Such as Android App [Torque Lite](https://play.google.com/store/apps/details?id=org.prowl.torque&hl=de), [Car Scanner ELM OBD2](https://play.google.com/store/apps/details?id=com.ovz.carscanner&hl=de), 
action cameras like "Garmin Virb X / XE" or other solutions, which are able to communicate with an OBD II Bluetooth Adapter.

# Update 2023/01/01
My latest so called "Bike Diagnostic System" version for Kawasaki & Suzuki. It´s almost compatible to any ELM327 App and supports nearly any KDS and SDS bike.
It has tons of settings, parameter, modes and an error log.

### Repository
* [ECU Emulation](https://github.com/HerrRiebmann/KDS2Bluetooth/tree/master/ECU%20Emulation) - Visual Studio C# Project to emulate the ECU
* [Bike_Diagnostic_System](https://github.com/HerrRiebmann/KDS2Bluetooth/tree/master/Bike_Diagnostic_System)    - Arduino Sketch -> Full solution (new)
* [ECU_Reader](https://github.com/HerrRiebmann/KDS2Bluetooth/tree/master/ECU_Reader)    - Arduino Sketch -> Full solution (old)
* [HC06_Sniffer](https://github.com/HerrRiebmann/KDS2Bluetooth/tree/master/HC06_Sniffer)  - Arduino Sketch to find out Bluetooth AT-commands
* [Documentation](https://github.com/HerrRiebmann/KDS2Bluetooth/tree/master/Documentation) - OBD / KDS PID-List, Schematics, ect.

### History:
Against all cars and trucks, the diagnostic interface is not standardized on two wheels.
Neither the plug, nor the communication.
Since ~1996 ODB and later on OBD II interface entered nearly every car. The motorbike manufactures are cooking their very own soup.

That´s why I cannot use a cheap (*china*) OBD II to Bluetooth adapter, to transfer RPM, Speed, Temperatures, Throttle, ect.
to my Garmin Virb XE action cam.
Also my bike doesn´t have any gear indicator. So I´d like to build my very own.

### Protocol:
KDS Bus via K-LINE (*Diagnostic plug*).
In my case KWP2000 (*ISO-14230*)

### Hardware:
* Arduino Nano
* HC-06 Bluetooth
* L9637D
* Ceramic-Capacitor 10 nF 50 V/DC
* 510Ohm Resistor
* SOIC Board (*to solder L9637 SMD on it*)

The L9637 converts the one wire K-Line bidirectional into a serial signal (Rx & Tx).
Connect Rx to Rx, Tx to Tx and VCC to 5V Output from Arduino. VCC, Gnd and K-Line to the Bike. A PullUp-resistor is needed between power supply (VCC) and K-Line!
HC-06 is a slave Bluetooth device. If needed, you can use a HC-05 master.

### Software:
This has to be splitted into three parts:

#### AT-Communication:
Nearly all bluetooth OBD dongles are using an ELM327 chip, or a similar, cheap china clone.
This is controlled/setup via "AT"-commands. They describe the format how data is transmitted, timing, reset the device or return protocol information.

#### KDS-Communication:
Connection to the kawasaki diagnostic system. How the Arduino speaks to the Bike.
Based on the great work of Tom Mitchell (*[Kawaduino](https://bitbucket.org/tomnz/kawaduino/overview)*).

#### Translations:
Every ELM327 compatible device is using the stock [OBD II protocol](https://en.wikipedia.org/wiki/OBD-II_PIDs) and its Protocol ID´s (*PID*).
The bike is also using a similar communication. But the PID´s doesn´t mean the same, nor is the calcualtion equal.
In that conclusion, the requested PID has to be translated into the according pendant and the result has to be calculated,
to fit into the OBD ranges.


### The most influential Tutorials/sources:
[KDS Protocol](https://web.archive.org/web/20201202041417/https://ecuhacking.activeboard.com/t56234221/kds-protocol/) (inactive ecuhacking.activeboard via archive.org)

[Kawaduino](https://github.com/tomnz/kawaduino)

[ELM327 AT Commands](https://www.elmelectronics.com/wp-content/uploads/2016/07/ELM327DS.pdf)

[OBDuino](https://en.wikipedia.org/wiki/OBDuino)

### My Cameras:
[Garmin Virb XE](http://virb.garmin.com/en-US/virb-xe)
[Garmin Virb X](http://virb.garmin.com/en-US/virb-x)

### My Videos (*more to come*)
[![Demo](https://img.youtube.com/vi/MKdlcnXseew/0.jpg)](https://www.youtube.com/watch?v=MKdlcnXseew&feature=youtu.be)

[![Racetrack Mettet](https://img.youtube.com/vi/7NzQdNrY6Ro/0.jpg)](https://youtu.be/7NzQdNrY6Ro?t=36s)

[My YouTube Channel](https://www.youtube.com/channel/UCT6vRSUNHg8XRwPQtbxhyKw)

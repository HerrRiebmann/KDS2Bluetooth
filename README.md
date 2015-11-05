# KDS2Bluetooth
Kawasaki Diagnostic Service Reader via Bluetooth

Using an Arduino (*Nano*) to read parameter from a Kawasaki motorcycle by it´s diagnostic system.
Requests can be submittet via bluetooth, by any EM327 compatible device/application. Such as Android App [Torque Lite](https://play.google.com/store/apps/details?id=org.prowl.torque&hl=de),
action cameras like "Garmin Virb X / XE" or other solutions, which are able to communicate with an OBD II Bluetooth Adapter.

##Repository
* ECU Emulation - Visual Studio C# Project to emulate the ECU
* ECU_Reader    - Arduino Sketch -> Full solution
* HC06_Sniffer  - Arduino Sketch to find out Bluetooth AT-commands

##History:
Against all cars and trucks, the diagnostic interface is not standardized on two wheels.
Neither the plug, nor the communication.
Since ~1996 ODB and later on OBD II interface entered nearly every car. The motorbike manufactures are cooking their very own soup.

That´s why I cannot use a cheap (*china*) OBD II to Bluetooth adapter, to transfer RPM, Speed, Temperatures, Throttle, ect.
to my Garmin Virb XE action cam.
Also my bike doesn´t have any gear indicator. So I´d like to build my very own.

##Protocol:
KDS Bus via K-LINE (*Diagnostic plug*).
In my case KWP2000 (*ISO-14230*)

##Hardware:
* Arduino Nano
* HC-06 Bluetooth
* L9637D
* Ceramic-Capacitor 10 nF 50 V/DC
* 510Ohm Resistor
* SOIC Board (*to solder L9637 SMD on it*)

The L9637 converts the one wire K-Line bidirectional into a serial signal (Rx & Tx).
The capacitor reduces the 12V (*or more while charging*) to 6V to supply the Arduino VIN. A PullUp-resistor is needed between
power supply and K-Line.
HC-06 is a slave Bluetooth device. If needed, you can use a HC-05 master.

##Software:
This has to be splitted into some parts:

###AT-Communication:
Nearly all bluetooth OBD dongles are using an ELM327 chip, or a similar, cheap china clone.
This is controlled via "AT"-commands.

###KDS-Communication:
Connection to the kawasaki diagnostic system.
Based on the great work of Tom Mitchell (*Kawaduino*).

###Translations:
Every ELM327 compatible device is using the stock [OBD II protocol] (https://en.wikipedia.org/wiki/OBD-II_PIDs) and its Protocol ID´s (*PID*).
The bike is also using a similar communication. But the PID´s doesn´t mean the same, nor is the calcualtion equal.
In that conclusion, the requested PID has to be translated into the according pendant and the result has to be calculated,
to fit into the OBD ranges.

###Display:
To show the current gear, connection state (*Bluetooth & ECU*), Voltage, etc. I´ll implement a OLED display soon.

##The most influential Tutorials/sources:
[KDS Protocol](http://ecuhacking.activeboard.com/t56234221/kds-protocol/?page=1&sort=oldestFirst)
[Kawaduino](https://bitbucket.org/tomnz/kawaduino/overview)
[ELM327 AT Commands](http://www.elmelectronics.com/ELM327/AT_Commands.pdf)
[OBDuino](https://en.wikipedia.org/wiki/OBDuino)

##My Camera:
[Garmin Virb XE](http://virb.garmin.com/en-US/virb-xe)
##My Bike:
[Kawasaki Z750r Black edition](http://www.kawasaki.de/de/products/sportler/2012/z750r_black_edition/overview?Uid=05D9WlhZXFhaWVhZXFpdWl1aUV5ZWF9eRgwNRSwt)
[Gallery](http://www.z1000-forum.de/garage/vehicle/6689-kawasaki-z750r-black-edition/?tab=images)

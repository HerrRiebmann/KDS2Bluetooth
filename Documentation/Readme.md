# Documentation
* L9637 converts the K-Line one-wire to a serial signal, which is readable by Arduino.
It´s functionality and wiring is explained in [L9637D IC_INTERFACE_BUS_ISO_E_L9637D_SOIC_8_STM.pdf](https://github.com/HerrRiebmann/KDS2Bluetooth/blob/master/Documentation/L9637D%20IC_INTERFACE_BUS_ISO_E_L9637D_SOIC_8_STM.pdf).

* The communication protocol from regular Bluetooth OBD II dongles is explained in [ELM327DS Schnittstellenbeschreibung.pdf](https://github.com/HerrRiebmann/KDS2Bluetooth/blob/master/Documentation/ELM327DS%20Schnittstellenbeschreibung.pdf).
To make Apps like [Torque Lite](https://play.google.com/store/apps/details?id=org.prowl.torque&hl=de) or [Car Scanner](https://www.microsoft.com/de-de/store/apps/car-scanner-elm-obd2/9nblggh5lpx9) speaking with the device, you will have to refer on that.

* [iso.14230-4.2000.pdf](https://github.com/HerrRiebmann/KDS2Bluetooth/blob/master/Documentation/iso.14230-4.2000.pdf) is the protocol used by KDS: Keyword protocol 2000 (ISO 14-230-4)
This is the predecessor of [Unified Diagnostic Services](https://en.wikipedia.org/wiki/Unified_Diagnostic_Services)

* [OBD II PIDs.xlsx](https://github.com/HerrRiebmann/KDS2Bluetooth/blob/master/Documentation/OBD%20II%20PIDs.xlsx) represents a list of known PID´s and SID´s. Also some of my testing and calculation results are included. But don´t take them for granted :)

* Last but not least, the pinout from the plug:
![Pin Layout](https://github.com/HerrRiebmann/KDS2Bluetooth/blob/master/Documentation/Pin%20Layout.jpg)
Possible Source of supply [here](http://www.corsa-technic.com/item.php?item_id=341)

* Often asked for a schmeatic of connecting the L9637D Serial to K-Line converter:
![L9631DSchematic](https://github.com/HerrRiebmann/KDS2Bluetooth/blob/master/Documentation/Schematic%20L9637D.png)

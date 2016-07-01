#ECU Emulation

A .Net Solution to simulate the ECU and fake some values to test the Arduino via USB Serial COM Port.

**Important:** If the L9637D IC is connected to Rx & Tx, you have to remove Rx!!!
Else it is not possible to upload a sketch or use the Serial Port.

*The best solution is to solder a switch between Rx and IC*

The solution is based on the need to test some values without having a bike present. It grew very fast and is not very flexible and clean coded.
When I find some time, I will rewrite it and make the setup more dynamically to support different Service ID´s and add Formulars to fake PID values.
But currently it will cover my needs and helped great through developing :)

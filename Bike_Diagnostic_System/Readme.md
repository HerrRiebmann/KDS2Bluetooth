# Bike Diagnostic System
Suzuki and Kawasaki compatible solution.

## AT Commands - ELM327 compatible
| Type  | Command | Description | Answer | Function
| ------------- | ------------- | ------------- | ------------- | ------------- |
|AT	|@	|1,2,3	Describe Device (Name, Description, New)	|OK	|Nothing
|AT	|AL		|Long Message Length	|OK	|Nothing
|AT	|AT	0,1,2	|Adaptive Timing	|OK	|Set adaptiveTiming
|AT	|AR		|Autoreceive	|OK	|Nothing
|AT	|BD		|Buffer Dump	|Buffer + OK	|Print BT & ECU Buffer
|AT	|BI	,0,1	|Bypass Initialization	|OK	
|AT	|D		|Reset to default	|OK	|ResetToDefault
|AT	|D	0,1	|Display DLC off,on		|Nothing
|AT	|DP		|Describe Protocol	|Protocol + OK	|PrintProtocol
|AT	|DPN		|Describe Protocol by Number	|Prot. No. + OK	
|AT	|E	0,1	|Echo	|OK	
|AT	|FI		|Fast Init	|BUS INIT: OK	
|AT	|H	0,1	|Header	|OK	
|AT	|I		|Device Info	|ELM Version	
|AT	|IB	10,48,96	|Baudrate 10400,4800,9600		|Nothing
|AT	|L	0,1	|Linefeed	|OK	
|AT	|M	0,1	|Memory	|OK	
|AT	|NL		|Normal Message Length	|OK	|Nothing
|AT	|PC		|ProtocolClose	|OK	|StopComm
|AT	|RA	0x00	|Set Receive Adress	|OK	
|AT	|RV		|Device Voltage	|12.3V	|BtPrintBattery
|AT	|S	0,1	|Spaces	|OK	
|AT	|SH	0x00 0x00 0x00	|Set Custom Header (Format, Receiver, Sender)	|OK	|Change Header
|AT	|SI		|Slow Init	|BUS INIT: OK	|Slow Init
|AT	|SP	,0,1,2,3,4,5,6	|Protocol Auto, Set Protocol	|OK	
|AT	|SPA	,0,1,2,3,4,5,6	|Set Protocol to Auto Protocol	|OK	
|AT	|SR	0x00	|Set Receive Adress	|OK	
|AT	|ST	XX	|Timer before "NO DATA"	|OK	|Nothing
|AT	|SW	0x00	|SetWakeup (Keepalive) interval	|OK	|Nothing
|AT	|TA	0x00	|Set Tester Adress	|OK	|Sender Adress
|AT	|WM	0x00 0x00 0x00	|Set WakeUp Message (max 3 byte)		
|AT	|WS		|WarmStart	|ELM Version	|ResetToDefault
|AT	|Z		|Reset/Restart	|ELM Version + OK	|ResetToDefault

## BT Commands - Bluetooth
| Type  | Command | Description 
| ------------- | ------------- | ------------- 
|BT	|+ADDR		|Default Adress		
|BT	|+BAUD	4,6,7,8	|SetBaudrate		
|BT	|+NAME		|See/Set Name		
|BT	|+ORGL		|Restore factory settings		
|BT	|+PSWD		|See/Set Password		
|BT	|+RESET		|Restart		
|BT	|+ROLE		|Master/slave		
|BT	|+UART		|Baudrate		
|BT	|+VERSION		|Version		
|BT	|		|Connection Check

## XT Commands - Custom setup/functions
| Type  | Command | Description | Answer | Function
| ------------- | ------------- | ------------- | ------------- | ------------- |
|XT		|0,1	|Status LED Off, On		
|XT	|A	,0,1	|Sniff All supported PID´s (Optional without check)		||SniffSupportedPIDs
|XT	|B		|Check Bluetooth Version & Store		||BtGetVersion
|XT	|C		|Clear Setup Values	|OK	|ResetStorage
|XT	|CS	0,1	|CheckSumMode		
|XT	|D		|Show all Diagnostic values		|DTC
|XT	|DC		|Clear Diagnostic|OK	|ClearDTC
|XT	|E		|Print all Errors		||PrintErrorMessages
|XT	|ER		|Reset all Errors	|OK	|ResetErrors
|XT	|EC	0,1	|Set ||EcuConnected		
|XT	|EL		|Show EEPROM Length		
|XT	|F		|Fetch Data (Sniffer)	|Yes/No	
|XT	|G		|Show Gearratio or ECU data		
|XT	|G	0,1	|Enable Autogear		||SetAutoGear
|XT	|I	M0|1,S,E,0,1	|ManualInit		||InitializeKLine
|XT	|K		|K-Line Test (Timing for Oscilator)		||TestKline
|XT	|K	0,1	|Keepalive	|OK	
|XT	|L	optional,0,1	|SD Logger is active		||CheckLoggerConnection
|XT	|O	0x00	|Set EEPROM Offset		||SetOffset
|XT	|P	0x00	|ManufacturerProtocol		||SetManufacturerProtocol
|XT	|R		|Show free RAM		||PrintFreeRam
|XT	|R	0,1	|Raw		||PrintRaw
|XT	|S		|Show Setup		||PrintAll
|XT	|S	0,1	|Set Temporary Storage		||PrintUseStorage
|XT	|T	0,1	|TestMode BT-Output		||PrintDebug
|XT	|X	|optional Int	Testfunction for specific tests		||TestSpecific()
|XT	|Z		|TestMode (ECU Id, DealerMode, All PID´s)		||TestBike()

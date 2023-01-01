struct CmdResult {
  bool Ok;
  bool Processed;
  CmdResult()
    : Ok(false), Processed(false) {
  }
  CmdResult(bool ok, bool processed) {
    Ok = ok;
    Processed = processed;
  }
};

bool ProcessAtCommand() {
  struct CmdResult result = RespondAtCommand();
  if (!result.Processed) {
    BtPrint(F("?"));
    SetError(ERROR_Unknown_AT);
  }
  PrintFooter(result.Ok);
  return result.Processed;
}

bool ProcessXtCommand() {
  struct CmdResult result = RespondXtCommand();
  if (!result.Processed) {
    BtPrint(F("?"));
    SetError(ERROR_Unknown_XT);
  }
  PrintFooter(result.Ok);
  return result.Processed;
}

struct CmdResult RespondAtCommand() {
  //Describe Device (Name, Description, New):
  if (Compare("AT@i")) {
    switch (btBuffer[3]) {
      case '1':  //display the device description
        BtPrint(F("KDS/SDS to OBDII Adapter"));
        return CmdResult(false, true);
      case '2':  //display the device identifier
        BtPrint(F("BikeDiagnosticSystem"));
        return CmdResult(false, true);
      case '3':  //store the device identifier
        return CmdResult(true, true);
    }
  }

  //Long Message Length
  if (Compare("ATAL")) {
    return CmdResult(true, true);
  }

  //Adaptive Timing (0,1,2)
  if (Compare("ATATi")) {
    /*
      case 0: //Disable
      case 1: //Auto1 (Set by ST)
      case 2: //Auto2 (More agressive...)
    */
    adaptiveTiming = btBuffer[4] - 48;  //From Char to Int
    if (adaptiveTiming == 0 || adaptiveTiming == 1 || adaptiveTiming == 2)
      return CmdResult(true, true);
    return CmdResult(false, false);
  }

  //Auto Receive: Get all messages with correct receiver adress
  if (Compare("ATAR")) {
    //ToDo: Activate receiving without request
    return CmdResult(true, true);
  }

  //Buffer Dump
  if (Compare("ATBD")) {
    //Bluetooth Buffer
    BtPrint(F("BT: "));
    PrintBuffer();
    //Kline Buffer
    BtPrint(F("Kline: "));
    PrintKBuffer();
    return CmdResult(false, true);
  }

  //Bypass Initialization
  if (Compare("ATBI")) {
    bypassInit = true;
    return CmdResult(true, true);
  }

  //Bypass Initialization (0, 1)
  if (Compare("ATBIb")) {
    bypassInit = btBuffer[4] == '1';
    return CmdResult(true, true);
  }
  
  //CAN Auto Formatting (0, 1)
  if (Compare("ATCAFb")) {
    //autoFormatting = btBuffer[5] == '1';
    return CmdResult(true, true);
  }
  //CAN Flowcontrol (0, 1)
  if (Compare("ATCFCb")) {    
    return CmdResult(true, true);
  }
  //CAN Silent Monitoring (0, 1)
  if (Compare("ATCSMb")) {    
    return CmdResult(true, true);
  }

  //Reset to default
  if (Compare("ATD")) {
    ResetToDefault();
    return CmdResult(true, true);
  }
  //Display DLC on/off (0, 1):
  if (Compare("ATDb")) {
    return CmdResult(true, true);
  }

  //Describe Protocol:
  if (Compare("ATDP")) {
    PrintProtocol();
    return CmdResult(false, true);
  }

  //Describe Protocol by Number:
  if (Compare("ATDPN")) {
    //ISO 14230-4 KWP (fast init, 10.4 kbaud)
    //BtPrint(5);
    //Auto
    BtPrint(isoProtocol);
    return CmdResult(false, true);
  }

  //Echo (0,1):
  if (Compare("ATEb")) {
    echo = btBuffer[3] == '1';
    return CmdResult(true, true);
  }

  //Flow Control Set Mode
  if (Compare("ATFCSMi")) {    
    return CmdResult(true, true);
  }

  //Fast Init:
  if (Compare("ATFI")) {
    BtPrint(F("BUS INIT: "));
    return CmdResult(StartKLine(), true);
  }

  //Header (0,1):
  if (Compare("ATHb")) {
    header = btBuffer[3] == '1';
    return CmdResult(true, true);
  }

  //Device Info:
  if (Compare("ATI")) {
    BtPrint(elmVersion);
    return CmdResult(false, true);
  }

  //Set Baudrate (10,48,96) = 10400,4800,9600:
  if (Compare("ATIBhh")) {
    //ToDo: Or to ignore?
    return CmdResult(true, true);
  }

  //Keywords
  if (Compare("ATKW")) {    
    BtPrint("0");
    return CmdResult(false, true);
  }
  if (Compare("ATKWb")) {
    //Keyword check off/on
    return CmdResult(true, true);
  }

  //Linefeed (0, 1):
  if (Compare("ATLb")) {
    linefeed = btBuffer[3] == '1';
    return CmdResult(true, true);
  }

  //Memory (0, 1):
  if (Compare("ATMb")) {
    memory = btBuffer[3] == '1';
    return CmdResult(true, true);
  }

  //Normal message length:
  if (Compare("ATNL")) {
    return CmdResult(true, true);
  }

  //Protocol Close:
  if (Compare("ATPC")) {
    StopCommunication();
    return CmdResult(true, true);
  }

  //Set Receiver Adress:
  if (Compare("ATRAhh")) {
    ecuAddress = GetByteFromHexValues(btBuffer[4], btBuffer[5]);
    return CmdResult(true, true);
  }

  //Device Voltage:
  if (Compare("ATRV")) {
    PrintInternalVoltage();
    return CmdResult(false, true);
  }

  //Spaces (0, 1):
  if (Compare("ATSb")) {
    spaces = btBuffer[3] == '1';
    return CmdResult(true, true);
  }

  //Set Custom Header (Format, Receiver, Sender):
  if (Compare("ATSHhhhhhh")) {
    customFormat = GetByteFromHexValues(btBuffer[4], btBuffer[5]);
    ecuAddress = GetByteFromHexValues(btBuffer[6], btBuffer[7]);
    myAddress = GetByteFromHexValues(btBuffer[8], btBuffer[9]);
    return CmdResult(true, true);
  }
  //Short version:
  if (Compare("ATSHihh")) {
    return CmdResult(true, true);
  }

  //Slow Init:
  if (Compare("ATSI")) {
    BtPrint(F("BUS INIT: "));
    SlowInit();
    return CmdResult(false, true);
  }

  //Protocol Auto:
  if (Compare("ATSP") || Compare("ATTP")) {
    //0: Automatic
    //5: ISO 14230-4 KWP (fast init, 10.4 kbaud)
    isoProtocol = 5;
    return CmdResult(true, true);
  }
  //Protocol 0-9:
  if (Compare("ATSPi") || Compare("ATTPi")) {
    //0: Automatic
    //5: ISO 14230-4 KWP (fast init, 10.4 kbaud)
    isoProtocol = GetVal(btBuffer[4]);
    return CmdResult(true, true);
  }
  //Set Protocol Auto 0-9:
  if (Compare("ATSPAi") || Compare("ATTPAi")) {
    isoProtocol = GetVal(btBuffer[5]);
    return CmdResult(true, true);
  }

  //Set Receive Adress:
  if (Compare("ATSRhh")) {
    ecuAddress = GetByteFromHexValues(btBuffer[4], btBuffer[5]);
    return CmdResult(true, true);
  }

  //Timeout before sending "NO DATA":
  if (Compare("ATSThh")) {
    //ToDo: Make Timeout variable?!
    //ATST64
    return CmdResult(true, true);
  }

  //SetWakeup (Keepalive) Interval:
  if (Compare("ATSWhh")) {
    //ToDo: Make variable?
    return CmdResult(true, true);
  }

  //Set Tester Adress:
  if (Compare("ATTAhh")) {
    myAddress = GetByteFromHexValues(btBuffer[4], btBuffer[5]);
    return CmdResult(true, true);
  }

  //Set WakeUp Message (max 3 byte):
  if (Compare("ATWMhh") || Compare("ATWMhhhh") || Compare("ATWMhhhhhh")) {
    keepaliveMsg[0] = GetByteFromHexValues(btBuffer[4], btBuffer[5]);
    if (btBufferCounter >= 8)
      keepaliveMsg[1] = GetByteFromHexValues(btBuffer[6], btBuffer[7]);
    else
      keepaliveMsg[1] = 0xFF;
    if (btBufferCounter >= 10)
      keepaliveMsg[2] = GetByteFromHexValues(btBuffer[8], btBuffer[9]);
    else
      keepaliveMsg[2] = 0xFF;
    return CmdResult(true, true);
  }
  //Got msg with header: 80 12 F1 01 3E (+ Checksum)
  if (Compare("ATWMhhhhhhhhhh") || Compare("ATWMhhhhhhhhhhhh")) {
    keepaliveMsg[0] = GetByteFromHexValues(btBuffer[12], btBuffer[13]);
    keepaliveMsg[1] = 0xFF;
    keepaliveMsg[2] = 0xFF;
    return CmdResult(true, true);
  }

  //WarmStart:
  if (Compare("ATWS")) {
    ResetToDefault();
    BtPrint(elmVersion);
    return CmdResult(false, true);
  }

  //Reset / Restart
  if (Compare("ATZ")) {
    if (ecuConnected)
      StopCommunication();
    ResetToDefault();
    BtPrintln(elmVersion);
    //Test for SZ Viewer
    return CmdResult(false, true);
    //return CmdResult(true, true);
  }
  return CmdResult(false, false);
}

struct CmdResult RespondXtCommand() {

  //LED Test
  if (Compare("XTb")) {
    SetStatusLed(btBuffer[2] == '1');
    return CmdResult(false, true);
  }

  //All supported PID´s
  if (Compare("XTA")) {
    SniffSupportedPIDs();
    return CmdResult(false, true);
  }
  if (Compare("XTAb")) {
    SniffSupportedPIDs(btBuffer[3] == '1');
    return CmdResult(false, true);
  }

  //Bluetooth Version
  if (Compare("XTB")) {
    WaitForDisconnect();
    BtGetVersion();
    PrintBtSetup();
    return CmdResult(false, true);
  }
  //Set Bluetooth Version manually
  if (Compare("XTBi")) {
    btVersion = btBuffer[3] - 48;
    SetBtVersion();
    PrintBtSetup();
    return CmdResult(false, true);
  }

  //Clear Setup Values
  if (Compare("XTC")) {
    ResetStorage();
    return CmdResult(true, true);
  }

  //Checksum Mode
  if (Compare("XTCSb")) {
    defaultCheckSum = btBuffer[4] == '1';
    return CmdResult(true, true);
  }

  //Read Diagnostic Values
  if (Compare("XTD")) {
    DTC();
    PrintDTC();
    return CmdResult(false, true);
  }

  //Clear Diagnostic Command
  if (Compare("XTDC")) {
    ClearDTC();
    return CmdResult(true, true);
  }

  //Show Error
  if (Compare("XTE")) {
    PrintErrorMessages();
    return CmdResult(false, true);
  }

  //Error Reset
  if (Compare("XTER")) {
    ResetErrors();
    return CmdResult(true, true);
  }

  //ECUConnected
  if (Compare("XTECb")) {
    ecuConnected = btBuffer[4] == '1';
    return CmdResult(false, true);
  }

  //EEPROM Length
  if (Compare("XTEL")) {
    //BtPrint("EEPROM Length");
    BtPrint(String(EEPROM.length()));
    BtPrintln();
    return CmdResult(false, true);
  }

  //Fetch data
  if (Compare("XTF")) {
    fetchMode = !fetchMode;
    if (fetchMode) {
      SetKline(LOW);
      StartSerial();
      rawMode = false; //Does not work in combination
      debugMode = true;
    }
    PrintBool(fetchMode);
    return CmdResult(false, true);
  }

  //GearRatio
  if (Compare("XTG")) {
    PrintBool(autoGear);
    return CmdResult(false, true);
  }
  if (Compare("XTGb")) {
    autoGear = btBuffer[3] == '1';
    SetAutoGear();
    PrintBool(autoGear);
    return CmdResult(false, true);
  }

  //Manual Init
  if (Compare("XTIMb")) {
    bypassInit = btBuffer[4] == '1';
    PrintBypassInit();
    if (bypassInit)
      InitializeKLine();
    return CmdResult(false, true);
  }
  if (Compare("XTIS")) {
    StartSerial();
    return CmdResult(false, true);
  }
  if (Compare("XTIE")) {
    EndSerial();
    return CmdResult(false, true);
  }
  if (Compare("XTIb")) {
    SetKline(btBuffer[3] == '1');
    return CmdResult(false, true);
  }

  if (Compare("XTIKi")) {
    switch (btBuffer[4]) {
      case '0':
        FastInit14230();
        break;
      case '1':
        FastInit9141();
        break;
      case '2':
        SlowInit();
        break;
    }
    return CmdResult(false, true);
  }

  //K-Line Test
  if (Compare("XTK")) {
    TestKline();
    return CmdResult(true, true);
  }

  //Keepalive
  if (Compare("XTKb")) {
    sendKeepAlive = btBuffer[3] == '1';
    return CmdResult(true, true);
  }

  //Set EEPROP Offset
  if (Compare("XTOhh")) {
    ResetStorage();
    eepromOffset = GetByteFromHexValues(btBuffer[3], btBuffer[4]);
    SetOffset();
    PrintOffset();
    return CmdResult(false, true);
  }

  //Bike Protocol
  if (Compare("XTPhh")) {
    bikeProtocol = GetByteFromHexValues(btBuffer[3], btBuffer[4]);
    SetBikeProtocol();
    SetManufacturerProtocol();
    PrintManufacturer();
    return CmdResult(false, true);
  }

  //Free RAM: ~884
  if (Compare("XTR")) {
    PrintFreeRam();
    return CmdResult(false, true);
  }
  //Raw Mode
  if (Compare("XTRb")) {
    rawMode = btBuffer[3] == '1';
    PrintRaw();
    return CmdResult(false, true);
  }
  //Show Setup
  if (Compare("XTS")) {
    PrintAll();
    return CmdResult(false, true);
  }

  //Set Temp Storage
  if (Compare("XTSb")) {
    tempStorage = btBuffer[3] == '1';
    PrintUseStorage();
    return CmdResult(false, true);
  }

  //Test-Mode
  if (Compare("XTTb")) {
    debugMode = btBuffer[3] == '1';
    PrintDebug();
    return CmdResult(false, true);
  }

  //TestSomething
  if (Compare("XTX")) {
    SetStatusLed(HIGH);
    TestSpecific();
    //delay(3000);
    //InitializeBluetooth();
    SetStatusLed(LOW);
    return CmdResult(false, true);
  }
  if (Compare("XTXi")) {
    SetStatusLed(HIGH);
    for(uint8_t i = 0; i <= btBuffer[3]; i++)
      TestSpecific();    
    SetStatusLed(LOW);
    return CmdResult(false, true);
  }
  if (Compare("XTZ")) {
    TestBike();
    return CmdResult(false, true);
  }

  return CmdResult(false, false);
}

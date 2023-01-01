void PrintAll() {
  PrintVersions();
  PrintAtSetup();
  PrintXtSetup();
  PrintGeneralInformation();
  PrintKeepalive();
  PrintBtSetup();
  PrintBtBaud();  
  PrintManufacturer();
  BtPrintln();  
  PrintProtocol();
  BtPrintln();
  PrintThrottleValues();
  //PrintErrorMessages();
  PrintInternalVoltage();
  PrintInternalTemp();
}

void PrintAtSetup() {
  BT.print(F("H: "));
  PrintBool(header);
  BT.print(F("S: "));
  PrintBool(spaces);
  BT.print(F("LF: "));
  PrintBool(linefeed);
  BT.print(F("Echo: "));
  PrintBool(echo);
  BT.print(F("Memory: "));
  PrintBool(memory);
  BT.print(F("Adap.Timing: "));
  BT.println(adaptiveTiming);
  BT.print(F("ISO: "));
  BT.println(isoProtocol);
  if (customFormat != 0xFF) {
    BT.print(F("Format: "));
    PrintHex(customFormat);
    BT.println();
  }
  BT.print(F("ECU: "));
  PrintHex(ecuAddress);
  BT.println();
  BT.print(F("Tester: "));
  PrintHex(myAddress);
  BT.println();
}

void PrintXtSetup() {
  PrintRaw();
  PrintDebug();
  PrintBypassInit();
  PrintUseStorage();
  PrintOffset();
}

void PrintRaw() {
  BT.print(F("Raw: "));
  PrintBool(rawMode);
}

void PrintDebug() {
  BT.print(F("Debug: "));
  PrintBool(debugMode);
}

void PrintBypassInit() {
  BT.print(F("BypassInit: "));
  PrintBool(bypassInit);
}

void PrintUseStorage() {
  BT.print(F("Storage: "));
  PrintBool(tempStorage);
}

void PrintOffset() {
  BT.print(F("Offset: "));
  BT.println(eepromOffset);
}

void PrintGeneralInformation() {
  BT.print(F("ECU: "));
  PrintBool(ecuConnected);  
  BT.print(lastKlineResponse);
  BT.print(F(" / "));
  BT.print(millis());
  BT.println(F("ms"));
}

void PrintBtSetup() {
  BT.print(F("BT V: "));
  BT.println(btVersion);
}

void PrintBtBaud() {
  BT.print(F("BT Baud: "));
  BT.println(baud);
}

void PrintManufacturer() {
  // Kawa1, Kawa2, Suzuki1, Suzuki2
  switch (bikeProtocol) {
    case 1:
      BT.print(F("Kawasaki "));
      break;
    case 2:
      BT.print(F("Kawasaki ABS "));
      break;
    case 3:      
    case 4:
      BT.print(F("Suzuki "));
      break;
    case 7:
      BT.print(F("Yamaha "));
      break;
    case 0xFF: //Not yet defined
      BT.print(F("undefined"));
      return;
    default:      
      BT.print(F("Unknown "));
      break;
  }
  BT.print(bikeProtocol);
}

bool PrintProtocol() {
  //0  Automatic protocol detection
  //1  SAE J1850 PWM (41.6 kbaud)
  //2 SAE J1850 VPW (10.4 kbaud)
  //3 ISO 9141-2 (5 baud init, 10.4 kbaud)
  //4 ISO 14230-4 KWP (5 baud init, 10.4 kbaud)
  //5 ISO 14230-4 KWP (fast init, 10.4 kbaud)
  //6 ISO 15765-4 CAN (11 bit ID, 500 kbaud)
  //7 ISO 15765-4 CAN (29 bit ID, 500 kbaud)
  //8 ISO 15765-4 CAN (11 bit ID, 250 kbaud) - used mainly on utility vehicles and Volvo
  //9 ISO 15765-4 CAN (29 bit ID, 250 kbaud) - used mainly on utility vehicles and Volvo
  switch (isoProtocol) {
    case 0:
      BT.print(F("AUTO, "));
      PrintIso();
      break;
    case 3:
      BT.print(F("ISO 9141-2 KWP"));
      break;
    case 4:
    case 5:
      PrintIso();
      break;
    case 6:
    case 7:
    case 8:
    case 9:
      BT.print(F("ISO 15765-4 CAN"));
      break;
    default:
      BT.print("?");
      return false;
  }
  return true;
}

void PrintIso(){
  BT.print(F("ISO 14230-4 KWP"));
}

void PrintThrottleValues() {
  BT.print(F("Throttle: "));
  BT.print(throttlePosMinimum);
  BT.print(F(" - "));
  BT.println(throttlePosMaximum);
  BT.print(F("Sub: "));
  BT.print(subThrottleMinimum);
  BT.print(F(" - "));
  BT.println(subThrottleMaximum);
}

void PrintKeepalive() {
  BT.print(F("KA: "));
  PrintBool(sendKeepAlive);  
  for (uint8_t i = 0; i < sizeof(keepaliveMsg); i++)
    if (keepaliveMsg[i] != 0xFF)
      PrintHex(keepaliveMsg[i]);
  BT.println(); 
}

void PrintFreeRam() {
  BT.print(F("RAM: "));
  BT.println(freeRam());
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void PrintVersions(){
  BT.print(F("Board: "));
  BT.println(BoardVersion);
  BT.print(F("FW: "));
  BT.println(FWVersion);
}

void PrintInternalVoltage(){
  PrintLongInDec(readVcc());  
  BtPrintln("V");
}

void PrintInternalTemp(){
  PrintLongInDec(readTemp());  
  BtPrintln("C");
}

void PrintLongInDec(long value){
  float f = (float)value / 1000;
  char str[5];  
  dtostrf(f, value < 10000 ? 3 : 4, 1, str);  
  BtPrint(str);
}

void PrintBool(bool b) {
  BT.println(b ? F("True") : F("False"));
}

// prints 8-bit data in hex with leading zeroes
void PrintHex(uint8_t data) {
  BT.print(GetHex(data));
}

void PrintFooter(bool ok) {
  if (ok)
    PrintOK();
  PrintNewLine();
  PrintPrompt();
}

void PrintNewLine() {
  //Carriage Return 13
  BtWrite(0x0D);
  //LineFeed 10
  if (linefeed)
    BtWrite(0x0A);
}

void PrintPrompt() {
  //Prompt: >
  BtWrite(0x3E);
}

void PrintSpace() {
  if (spaces)
    BtWrite(0x20);
}

void PrintOK() {
  BtPrint(F("OK"));
}

void PrintElmInitError() {
  BtPrint(F("BUS INIT:"));

  //only on slowInit
  if (isoProtocol != 0 && isoProtocol != 5)
    BtPrint(F("..."));
  BtPrintln(F("ERROR"));

  //CRC
  //ERROR
  //TIMEOUT
}

void PrintBuffer() {
  for (int i = 0; i < btBufferCounter; i++) {
    if (IsHexRequest) {
      PrintHex(btBuffer[i]);
      if (i < btBufferCounter - 1)
        PrintSpace();
    }
    else
      BtWrite(btBuffer[i]);
  }
  BtPrintln();
}

void PrintKBuffer() {
  for (int i = 0; i < ecuBufferInCounter; i++)
    PrintHex(ecuBufferIn[i]);
  BtPrintln();
}

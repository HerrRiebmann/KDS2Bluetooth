void SniffSupportedPIDs() {
  SniffSupportedPIDs(false);
}
void SniffSupportedPIDs(bool everything) {
  uint8_t pidList[] = { 0x00, 0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0};
  uint8_t pidListCount = (uint8_t)(sizeof(pidList));

  //PID (0x00 is already in pidList)
  data.PID = 0x01;

  if (!ecuConnected)
    if (!StartKLineConnection())
      return;

  //Get first PID List
  SniffCommand(pidList[0]);

  for (int i = 0; i < pidListCount; i++)  {
    //Copy Array  from last PID List before it will be overwritten:
    uint8_t ecuResponseCopy[11];
    memcpy(ecuResponseCopy, ecuBufferIn, 11);
    //Result: PID List
    //80 F1 11 06 61 00 DF F7 87 87 CD
    //                  DF F7 87 87
    //1101 1111 1111 0111 1000 0111 1000 0111
    //01 02 XX 04 05 06 07 08 09...
    //For each Hex value in ECUresponse
    for (uint8_t j = 6; j < 10; j++)    {
      byte bin = ecuResponseCopy[j];
      //0x80 = 1000 0000 | Shift 1 to the right
      for (int mask = 0x80; mask != 0; mask >>= 1) {
        if (bin & mask || everything)
          SniffCommand(data.PID);
        data.PID++;
      }
    }
  }
}

void SniffCommand(uint8_t pid) {
  SniffCommand(0x21, pid);
}

void SniffCommand(uint8_t sid, uint8_t pid) {
  if (!debugMode) {
    BtPrint(F(">"));
    BtPrintln(GetHex(pid));
  }
  uint8_t reqLen = 2;
  uint8_t req[2];
  uint8_t respLen;
  req[0] = sid;
  req[1] = pid;

  SendToKline(req, reqLen);
  respLen = ReceiveKlineResponse();

  if (debugMode)
    return;
  BtPrint(F("<"));
  for (int i = 0; i <= respLen; i++)
    PrintHex(ecuBufferIn[i + 3]);
  BtPrintln();
}

void TestBike() {
  PrintManufacturer();
  if (!ecuConnected)
    if (!StartKLineConnection())
      return;

  //ECU ID
  data.TranslatedSID = 0x1A;
  EcuId();
  SendToKline();
  if (ReceiveKlineResponse() > 0 && !debugMode)
    RespondFromEcu();

  //Show Diagnostic
  DTC();
  if (!debugMode)
    PrintDTC();

  //Show Freezed Frames
  data.SID = 0x01;
  data.PID = 0x02;
  FreezedFrames();
  data.TranslatedSID = 0x12;
  RespondFromStorage();

  //Sniff all
  SniffSupportedPIDs(true);

  //Sniff Freezed Frames (Kawa):
  if (IsKawasaki())
    KawaAllFreezedFrames();
}

void TestSpecific() {
  if (!ecuConnected)
    if (!StartKLineConnection())
      return;

  if (IsSuzuki()) {
    //Reference:
    SniffCommand(0x08);
    //Unknown
    SniffCommand(0x80);
    SniffCommand(0x90);
    SniffCommand(0xC0);
  }

  if (IsKawasaki()) {
    //Reference
    SniffCommand(0x04);
    SniffCommand(0x05);
    SniffCommand(0x06);
    SniffCommand(0x07);
    SniffCommand(0x08);
    SniffCommand(0x09);
    SniffCommand(0x0A);
    SniffCommand(0x0B);
    SniffCommand(0x0C);
    //Unknown PID with a lot of values...
    SniffCommand(0xBF);

    //    for (uint8_t i = 0; i < 50; i++) {
    //      //ABS:
    //      //SniffCommand(0x01);
    //      SniffCommand(0x02);
    //      SniffCommand(0x03);
    //      SniffCommand(0x04);
    //      SniffCommand(0x05);
    //      //SniffCommand(0x83);
    //      delay(200);
    //    }
  }
}

void FetchData() {
  if (!fetchMode)
    return;

  uint8_t BitDelay = 40;

  while (Serial.available()) {
    long messageStart = millis();
    ecuBufferIn[ecuBufferInCounter++] = Serial.read();

    //Init sometimes start with zeros (from fast init?!)
    if (ecuBufferInCounter == 1 && ecuBufferIn[0] == 0x00) {
      ecuBufferInCounter--;
      break;
    }
    //Buffer exeeded
    if (ecuBufferInCounter >= BUFFER_SIZE)
      break;
    //Message length and checksum complete received:
    if (MessageComplete())
      break;

    //Delay    
    for (uint8_t i = 0; i <= BitDelay; i++) {
      if (Serial.available()) {
        BitDelay = millis() - messageStart;
        BitDelay += 5;        
        break;
      }
      delay(1);
    }
  }
  if (ecuBufferInCounter < 1)
    return;
  PrintKBuffer();

  ClearKlineBuffer();
  lastKlineResponse = millis();
}

bool MessageComplete() {
  uint8_t formatPhysical = 0x81;
  uint8_t expectedLength = 0;

  //0x81 message (fixed length: 1, Target & Receiver)
  if ((ecuBufferIn[0] & formatPhysical) == formatPhysical)
    expectedLength = 5; //4 + 81 - 80 = 5

  formatPhysical--;
  //0x80 message (variable length)
  if (ecuBufferIn[0] == formatPhysical && ecuBufferInCounter > 4) //Format, Receiver, Sender, Length
    expectedLength = ecuBufferIn[3] + 5; //Format, Receiver, Sender, Length, CKS

  //0xC0 message (variable length)
  uint8_t masked = ecuBufferIn[0] & 0x3F;
  if (expectedLength == 0 && masked != 0)
    expectedLength = masked;

  //Length in 2nd byte
  if (expectedLength == 0 && ecuBufferInCounter > 1)
    expectedLength = ecuBufferIn[1];
  if (expectedLength > 0)
    return expectedLength == ecuBufferInCounter;
  return false;
}

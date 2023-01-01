void ReadAvailableInput() {
  ReadBluetooth();
  ReadKline();
}

bool ReadBluetooth() {
  if (!BT.available())
    return false;

  lastBtRequest = millis();
  while (BT.available() && millis() - lastBtRequest < BT_Timeout) {
    ProcessBt(BT.read());
  }
  return true;
}

void ProcessBt(char input) {
  //+DISC:SUCCESS
  if (input == '+' && btBufferCounter == 0) {
    ProcessBtResponse(input);
    return;
  }
  switch (input) {
    case ' ':   //Space (0x20)
    case 0x0A:  //Linefeed
      return;
    case 0x0D:  //Carriage Return -> Command complete
      ProcessBtBuffer();
      break;
    default:
      AddToBtBuffer(input);
      break;
  }
}

bool ReadKline() {
  //Data on Kline without a request
  if (!Serial.available())
    return false;

  //ToDo: do something with unrequested data
  //ATAR -> Auto-Receive

  //If raw
  //Send Buffer with Header? Footer? Space? Checksum?
  if (rawMode) {
    ReceiveKlineResponse();
    RespondFromEcu();
    return true;
  }

  //Use as a sniffer (fetch all data):
  FetchData();

  //None of the above:
  //Fill Data storage with converted values

  //Answer to BT? With last requested SID & PID?
  //Only write to data storage!!!
  //Answers only on requests from Bluetooth! Not Keepalive, Init-Response, masked DTC requests, ect.
  //Maybe exceptions?!?
  return true;
}

void AddToBtBuffer(char input) {
  //Set all chars (a-z) to upper
  SetCharToUpper(input);

  //Store first char, when empty
  if (hexBuffer == 0xFF) {
    hexBuffer = input;
    return;
  }

  //Prevent buffer overflow
  if (btBufferCounter + 1 >= BUFFER_SIZE) {
    SetError(ERROR_BT_Buffer_Exeeded);
    ClearBtBuffer();
    return;
  }

  //Set request type once
  if (btBufferCounter == 0)
    IsHexRequest = CheckIsHexCommand(hexBuffer, input);

  //If is Hex: Convert previous and current char to a combinded hex
  if (IsHexRequest) {
    btBuffer[btBufferCounter++] = GetByteFromHexValues(hexBuffer, input);
    hexBuffer = 0xFF;
    return;
  }

  //Else At, Xt, Bt
  //Don´t clear hexBuffer, to ignore combining two values
  if (btBufferCounter == 0)
    btBuffer[btBufferCounter++] = hexBuffer;
  btBuffer[btBufferCounter++] = input;
}

bool ProcessBtBuffer() {
  bool processed = false;

  //Repeat last command if available: (Just a linebreak appears)
  if (btBufferCounter == 0) {
    if (data.SID == 0xFF || data.PID == 0xFF || !ecuConnected)
      return processed;
    btBuffer[0] = data.SID;
    btBuffer[1] = data.PID;
    btBufferCounter = 2;
    IsHexRequest = true;
  }

  if (echo)
    PrintBuffer();

  switch (btBuffer[0]) {
    case 'A':
      processed = ProcessAtCommand();
      break;
    case 'B':
      processed = ProcessBtCommand();
      break;
    case 'X':
      processed = ProcessXtCommand();
      break;
    default:
      processed = ProcessKlineRequest();
      break;
  }

  //OK - Reponse from HC-06 Module
  if (!processed && Compare("OK")) {
    ProcessBtResponse('\0');
    return true;
  }

  //Any Command which does not contain Hex or AT, BT or XT:
  if (!processed && !IsHexRequest)
    if ((btBuffer[0] != 'A' || btBuffer[0] != 'B' || btBuffer[0] != 'X') && btBuffer[1] != 'T') {
      SetError(ERROR_Unknown_CMD);
      BtPrint(F("?"));
    }

  ClearBtBuffer();
  return processed;
}

void ClearBtBuffer() {
  memset(btBuffer, 0, sizeof(btBuffer));
  btBufferCounter = 0;
  hexBuffer = 0xFF;
  IsHexRequest = false;
  lastBtRequest = 0;
}

bool CheckInputTimeout() {  
  if (btBufferCounter == 0)
    return false;
  if (lastBtRequest == 0)
    return false;
  if (millis() - lastBtRequest > BT_Timeout) {
    ClearBtBuffer();
    BtPrint("?");
    SetError(ERROR_BT_Timeout);
    return true;
  }
  return false;
}

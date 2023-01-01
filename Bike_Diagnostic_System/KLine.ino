void InitializeKLine() {
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);
  ecuConnected = false;
  lastKlineResponse = 0;
}

bool ProcessKlineRequest() {
  if (!IsHexRequest)
    return false;

  //ToDo: Different order! Check command is valid/convert, ecuConnected?, Storage/Raw/Fake/Life/, answer by bluetooth or just store silent.
  if (!ecuConnected) {
    BtPrintln(F("BUSY"));
    if (!StartKLineConnection()) {
      BtPrintln(F("ERROR"));
      return false;
    }
  }

  ConvertFromObd();

  //Processed. Any error was catched in ConvertFromOBD
  return true;
}

bool StartKLineConnection() {
  if(bikeProtocol == 0 || bikeProtocol == 255){
    bikeProtocol = 1;
    InitializeManufacturer();
    if(StartKLineConnection())
      return true;
    delay(500);
    bikeProtocol = 3;
    InitializeManufacturer();
    if(StartKLineConnection())
      return true;
    bikeProtocol = 0;
  }
  if (StartKLine())
    return StartCommunication();
  return false;
}

bool StartKLine() {
  if (bypassInit)
    return true;

  EndSerial();

  switch (bikeProtocol) {
    case 0:
    case 1: //Kawasaki 1
    case 2: //Kawasaki 2
    case 3: //Suzuki 1
    case 4: //Suzuki 2
      return FastInit14230();
    case 5: //Honda 1
    case 6: //Honda 2
      return FastInit9141();
    default:
      SetError(ERROR_Unkonwn_InitProtocol);
      break;
  }
  return false;
}

bool StartCommunication() {
  if (bypassInit)
    return true;

  switch (bikeProtocol) {
    case 0:
    case 1: //Kawasaki 1
    case 2: //Kawasaki 2
      return ComStartKawasaki();
    case 3: //Suzuki 1
    case 4: //Suzuki 2
      return ComStartSuzuki();
    case 5: //Honda 1
    case 6: //Honda 2
    //ToDo: Honda & Shit
    //return ComStartHonda();
    default:
      SetError(ERROR_Unkonwn_InitProtocol);
      break;
  }
  return false;
}

bool StopCommunication() {
  if (bypassInit)
    return true;

  switch (bikeProtocol) {
    case 0:
    case 1: //Kawasaki 1
    case 2: //Kawasaki 2
      return ComStopKawasaki();
    case 3: //Suzuki 1
    case 4: //Suzuki 2
      return ComStopSuzuki();
    case 5: //Honda 1
    case 6: //Honda 2
    //ToDo: Honda & Shit
    //return ComStopHonda();
    default:
      SetError(ERROR_Unkonwn_InitProtocol);
      break;
  }
  return false;
}

// ECU Init Pulse (ISO 14230-2)
bool FastInit14230() {
  SetKline(HIGH);
  delay(300);
  SetKline(LOW);
  delay(25);
  SetKline(HIGH);
  delay(25);
  StartSerial();
  return true;
}

//Honda ISO-9141-2
//https://gonzos.net/projects/ctx-obd/
//https://github.com/mickwheelz/Honda-Motorcycle-Dash
bool FastInit9141() {
  SetKline(LOW);
  delay(70);
  SetKline(HIGH);
  delay(120);
  StartSerial();
  return true;
}

byte SlowInit() {
  byte b;
  byte kw1 = 0xFF;
  byte kw2 = 0xFF;
  //disable UART so we can "bit-Bang" the slow init.

  EndSerial();
  InitializeKLine();

  delay(3000); //k line should be free of traffic for at least two secconds.
  // drive K line high for 300ms
  SetKline(HIGH);
  delay(300);

  // send 0x33 at 5 bauds
  // start bit
  SetKline(LOW);
  delay(200);
  // data
  b = 0x33;
  for (byte mask = 0x01; mask; mask <<= 1) {
    if (b & mask) // choose bit
      SetKline(HIGH); // send 1
    else
      SetKline(LOW); // send 0
    delay(200);
  }
  // stop bit + 60 ms delay
  SetKline(HIGH);
  delay(260);

  // switch now to 10400 bauds
  StartSerial();

  // wait for 0x55 from the ECU (up to 300ms)
  //since our time out for reading is 125ms, we will try it three times
  for (int i = 0; i < 3; i++) {
    if (Serial.available())
      b = Serial.read();
    if (b != 0)
      break;
  }

  if (b != 0x55) {
    SetError(ERROR_SlowInit);
    return false;
  }

  // wait for kw1 and kw2
  //kw1=iso_read_byte();
  if (Serial.available())
    kw1 = Serial.read();

  //Response not empty:
  if (kw1 == 0xFF) {
    SetError(ERROR_SlowInit);
    return false;
  }

  //kw2=iso_read_byte();
  if (Serial.available())
    kw2 = Serial.read();
  //  delay(25);

  // sent ~kw2 (invert of last keyword)
  //iso_write_byte(~kw2);
  Serial.write(~kw2);

  // ECU answer by 0xCC (~0x33)
  //b=iso_read_byte();
  if (Serial.available())
    b = Serial.read();
  if (b != 0xCC) {
    SetError(ERROR_SlowInit);
    return false;
  }
  return true;
}

bool ComStartKawasaki() {
  uint8_t reqLen;
  uint8_t req[3];
  uint8_t respLen;

  // Start Communication is a single byte "0x81" packet.
  req[0] = 0x81;
  reqLen = 1;
  SendToKline(req, reqLen);
  respLen = ReceiveKlineResponse();

  // Response should be 3 bytes: 0xC1 0xEA 0x8F
  //0xEA (Explaines supported Header type: Source, Target, Length & Checksum)
  //additional length byte & normal timing
  if ((respLen == 3) && (ecuBufferIn[4] == 0xC1) && (ecuBufferIn[5] == 0xEA) && (ecuBufferIn[6] == 0x8F)) {
    // Success, so send the Start Diag frame
    // 2 bytes: 0x10 0x80
    req[0] = 0x10;
    req[1] = 0x80;
    reqLen = 2;
    //Start Diagnostic Session
    SendToKline(req, reqLen);
    respLen = ReceiveKlineResponse();
    // OK Response should be 2 bytes: 0x50 0x80
    //0x80 = Default diagnostic mode
    if ((respLen == 2) && (ecuBufferIn[4] == 0x50) && (ecuBufferIn[5] == 0x80)) {
      ecuConnected = true;
      //Immediately send Keepalive
      StayAlive();
      return ecuConnected;
    }
  }
  //Reset ECUConnected & LED + delay 5 Seconds
  SetError(ERROR_StartCom);
  ecuConnected = false;
  return false;
}

bool ComStartSuzuki() {  
  uint8_t reqLen = 1;
  uint8_t req[2];
  uint8_t respLen;
  // Start Communication is a single byte "0x81" packet.
  //81 means Format without Header Information (Length)
  req[0] = 0x81;
  SendToKline(req, reqLen);
  respLen = ReceiveKlineResponse();
  // Response should be 3 bytes: 0xC1 0xEA 0x8F
  //243 & 143 (Explaines supported Header type: Source, Target, Length & Checksum)
  if ((respLen == 3) && (ecuBufferIn[4] == 0xC1) && (ecuBufferIn[5] == 0xEA) && (ecuBufferIn[6] == 0x8F)) {
    ecuConnected = true;
    //Immediately send Keepalive
    StayAlive();    
    return ecuConnected;
  }
  //Reset ECUConnected & LED + delay 5 Seconds
  SetError(ERROR_StartCom);
  ecuConnected = false;
  return false;
}

bool ComStopKawasaki() {
  uint8_t reqLen;
  uint8_t req[1];

  // Stop Communication is a single byte "0x82" packet.
  req[0] = 0x82;
  reqLen = 1;
  SendToKline(req, reqLen);
  ecuConnected = false;
  SetStatusLed(LOW);

  uint8_t respLen = ReceiveKlineResponse();
  // OK Response should be 0xC2
  //80 F1 11 01 C2 45 (82 + 40)
  if (!(respLen == 1 && ecuBufferIn[4] == 0xC2)) {
    SetError(ERROR_StopCom);
    return false;
  }
  return true;
}

bool ComStopSuzuki()
{
  uint8_t reqLen;
  uint8_t req[1];

  // Stop Communication is a single byte "0x82" packet.
  req[0] = 0x82;
  reqLen = 1;
  SendToKline(req, reqLen);
  ecuConnected = false;
  SetStatusLed(LOW);

  uint8_t respLen = ReceiveKlineResponse();
  // OK Response should be 0xC2 (82 + 40)
  if (!(respLen == 1 && ecuBufferIn[4] == 0xC2)) {
    SetError(ERROR_StopCom);
    return false;
  }
  return true;
}

void SendToKline() {
  uint8_t reqLen = 2 + 5;//Add optional temp values
  if (rawMode)
    reqLen = btBufferCounter;

  uint8_t ecuBufferOut[reqLen];
  uint8_t ecuBufferCounter = 0;

  if (rawMode) {
    //Send Bluetooth buffer
    for (uint8_t i = 0; i < btBufferCounter; i++) {
      ecuBufferOut[ecuBufferCounter++] = btBuffer[i];
    }
  }
  else {
    ecuBufferOut[ecuBufferCounter++] = data.TranslatedSID;
    if (data.TranslatedPID != 0xFF)
      ecuBufferOut[ecuBufferCounter++] = data.TranslatedPID;
    //Attach optional values
    for (uint8_t i = 0; i < ecuBufferOutTmpCounter; i++)
      ecuBufferOut[ecuBufferCounter++] = ecuBufferOutTmp[i];
  }

  SendToKline(ecuBufferOut, ecuBufferCounter);
  ClearTempBuffer();
}

void SendToKline(uint8_t *request, uint8_t reqLen) {
  uint8_t newReqLen = reqLen + 6; //[SID + PID + 6]  Format, EcuAdress, MyAdress, Length, [data], Checksum, \0

  uint8_t ecuBufferOut[newReqLen];
  uint8_t ecuBufferCounter = 0;

  if (reqLen == 1 && (request[0] == 0x81)) //Only Init without length, stop diagnostic session (0x82) must be sent as 0x80
    ecuBufferOut[ecuBufferCounter++] = 0x81;
  else
    ecuBufferOut[ecuBufferCounter++] = 0x80;

  //If Custom Format Header is set, overwrite
  if (customFormat != 0xFF)
    ecuBufferOut[ecuBufferCounter - 1] = customFormat;

  ecuBufferOut[ecuBufferCounter++] = ecuAddress;
  ecuBufferOut[ecuBufferCounter++] = myAddress;

  if (ecuBufferOut[0] == 0x80)
    ecuBufferOut[ecuBufferCounter++] = reqLen;

  for (uint8_t i = 0; i < reqLen; i++) {
    ecuBufferOut[ecuBufferCounter++] = request[i];
  }

  ecuBufferOut[ecuBufferCounter] = CalcChecksum(ecuBufferOut, ecuBufferCounter);
  ecuBufferCounter += 1;
  SendKlineRequest(ecuBufferOut, ecuBufferCounter);
}

//Send Data from Buffer to ECU
void SendKlineRequest(uint8_t *request, uint8_t reqLen) {  
  SetStatusLed(HIGH);

  //wait until last request is older then ISORequestDelay
  if (millis() - lastKlineResponse < K_ISORequestDelay) {
    uint32_t waitIsoDelay = millis() - lastKlineResponse;
    if (K_ISORequestDelay - waitIsoDelay > 0)
      delay(K_ISORequestDelay - waitIsoDelay);
  }

  // Now send the command...
  for (uint8_t i = 0; i < reqLen; i++) {
    Serial.write(request[i]);
    delay(K_ISORequestByteDelay);
  }
}

//Receive Data from ECU to Buffer
//Returns Number of data-bytes returned
uint8_t ReceiveKlineResponse() {
  ClearKlineBuffer();
  uint8_t bytesToRcv = 0;
  uint8_t bytesRcvd = 0;
  uint32_t startTime = millis();
  bool forMe = false;

  // Wait for and deal with the reply
  while (millis() - startTime < K_MaxSendTime) {
    if (Serial.available()) {
      ecuBufferIn[ecuBufferInCounter] = Serial.read();
      if (debugMode)
        BtPrint(GetHex(ecuBufferIn[ecuBufferInCounter]));
      startTime = millis(); // reset the timer on each byte received
      switch (ecuBufferInCounter) {
        case 0:
          // should be an addr packet either 0x80 or 0x81
          //0x80 = length 0 | 0x81 = length 1
          bytesToRcv = ecuBufferIn[0] - 0x80;
          break;
        case 1:
          // should be the target address
          forMe = ecuBufferIn[1] == myAddress;
          break;
        case 2:
          // should be the sender address
          forMe = ecuBufferIn[2] == ecuAddress;
          // ignore the packet if is just an echo from our message!
          break;
        case 3:
          // should be the number of bytes, or the response if its a single byte packet.
          if (bytesToRcv == 1)
            bytesRcvd++;
          else
            bytesToRcv = ecuBufferIn[3]; // number of bytes of data in the packet.
          break;
        default:
          if (bytesToRcv == bytesRcvd) {
            if (debugMode)
              BtPrintln();
            // must be the checksum
            if (forMe) {
              // Only check the checksum if it was for us - don't care otherwise!
              if (CalcChecksum(ecuBufferIn, ecuBufferInCounter) == ecuBufferIn[ecuBufferInCounter]) {
                lastKlineResponse = millis();
                // Checksum OK.
                ecuBufferIn[++ecuBufferInCounter] = '\0';
                SetStatusLed(LOW);                
                return bytesRcvd;
              }
              else {
                //Checksum Error:
                SetError(ERROR_Checksum);
                return 0;
              }
            }
            //Not for me
            ClearKlineBuffer();
            bytesRcvd = 0;
            bytesToRcv = 0;
            ecuBufferInCounter = 0;
            //Skip counter
            continue;
          }
          else {
            // must be data, so put it in the response buffer
            bytesRcvd++;
          }
          break;
      }
      ecuBufferInCounter++;
      //Check Buffer exeeded!
      if (ecuBufferInCounter >= BUFFER_SIZE) {
        SetError(ERROR_K_Buffer_Exeeded);
        return bytesRcvd;
      }
    }
  }
  if (debugMode)
    BtPrintln();
  //Timeout: Reset ECUConnected & LED + delay 5 Seconds
  SetError(ERROR_Receive_Timeout);
  KlineErrorAppeared();
  return 0;
}

bool CheckIdle() {
  //ECU Timeout 2 Seconds: stay below
  return ((millis() - lastKlineResponse) > K_Keepalive);
}

void StayAlive() {
  if (!sendKeepAlive)
    return;
  if (!ecuConnected)
    return;

  //"Tester Present"
  //0x3E (SID)
  //Answer Positive 0x7E
  uint8_t reqLen;
  uint8_t req[3];
  uint8_t respLen;

  for (uint8_t i = 0; i < sizeof(keepaliveMsg); i++) {
    if (keepaliveMsg[i] != 0xFF) {
      req[i] = keepaliveMsg[i];
      reqLen = i + 1;
    }
  }
  SendToKline(req, reqLen);
  respLen = ReceiveKlineResponse();
  if ((respLen >= 1) && (ecuBufferIn[4] == 0x7E)) {
    if (debugMode)
      BtPrintln(F("KA OK"));
    return;
  }
  if ((respLen >= 2) && (ecuBufferIn[4] == 0x7F)) {
    SetError(ERROR_7F_Response_KA, ecuBufferIn[6]);
    ecuConnected = false;
    return;
  }
  if (debugMode) {
    BtPrintln(F("No KA"));
  }
  //If f.e. custom KA-Message and no 7F appears: Stay alive :)
  //ecuConnected = false;
}

// Checksum is simply the sum of all data bytes modulo 0xFF
// (same as being truncated to one byte)
uint8_t CalcChecksum(uint8_t *data, uint8_t len) {
  if (!defaultCheckSum)
    return CalcChecksum9141(data, len);
  uint8_t crc = 0;
  for (uint8_t i = 0; i < len; i++) {
    crc += data[i];
  }
  return crc;
}

//Honda ISO-9141-2
uint8_t CalcChecksum9141(uint8_t *data, uint8_t len) {
  uint16_t crc = 0;
  for (uint8_t i = 0; i < len; i++) {
    crc = crc + data[i];
  }
  uint8_t lb = lowByte(crc);
  return 0x100 - lb;
}

void KlineErrorAppeared() {
  //Init got its own error sequence
  if (ecuConnected)
    BtPrintln(F("ERROR"));
  SetStatusLed(LOW);
  ecuConnected = false;
  ClearKlineBuffer();
  ClearSerialBuffer();
  delay(K_ErrorTimeout);
}

void ClearKlineBuffer() {
  memset(ecuBufferIn, 0, BUFFER_SIZE);
  ecuBufferInCounter = 0;
}

void ClearTempBuffer() {
  for (uint8_t i = 0; i < 5; i++)
    ecuBufferOutTmp[i] = 0xFF;
  ecuBufferOutTmpCounter = 0;
}

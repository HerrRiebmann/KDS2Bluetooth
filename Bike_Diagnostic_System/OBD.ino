struct MessageLifespan {
  bool RequestToEcu = true;
  bool ResponseToBt = true;
  bool SkipPidTranslation = false;
  bool SkipSidTranslation = false;
  bool ResponseToBtFromStorage = true;
  bool ResponseToBtFromEcu = false;
  MessageLifespan() {
  }
  void Reset() {
    RequestToEcu = true;
    ResponseToBt = true;
    SkipPidTranslation = false;
    SkipSidTranslation = false;
    ResponseToBtFromStorage = true;
    ResponseToBtFromEcu = false;
  }
};
MessageLifespan msgLife;

void ConvertFromObd() {
  //Set Message livespan:
  msgLife.Reset();

  //Store SID & PID
  GetValuesFromBtRequest();

  if (rawMode) {
    SendToKline();
    ReceiveKlineResponse();
    RespondFromEcu();
    return;
  }

  //Return constant (fake) values
  if (FakeBtResponse())
    return;

  if (!msgLife.SkipSidTranslation)
    if (!TranslateBtRequestSid()) {
      //General reject
      SendBtError(0x10);
      return;
    }

  if (!msgLife.SkipPidTranslation)
    if (!TranslateBtRequestPid()) {
      //Service not supported
      SendBtError(0x11);
      return;
    }

  //Call direct ECU functions, fill storage otherwise or respond directly
  ProcessSpecificCalls();

  if (msgLife.RequestToEcu)
    msgLife.RequestToEcu = SendLiveRequest();

  if (msgLife.RequestToEcu) {
    SendToKline();
    //Receive from Kline (Return if empty)
    if (ReceiveKlineResponse() == 0)
      return;
    //Check 7F
    if (CheckForError())
      return;
    //Write To Storage
    TransferToStorage();
  }

  //Answer from Storage
  if (msgLife.ResponseToBtFromStorage)
    if (!RespondFromStorage()) {
      //Sub Function Not Supported - Invalid Format
      SendBtError(0x12);
      return;
    }

  //Answer from ECU stack
  if (msgLife.ResponseToBtFromEcu)
    RespondFromEcu();
}

bool SendLiveRequest() {
  if (!tempStorage || rawMode || data.Uninitialized())
    return true;

  //ToDo: Check if data should be read from storage only
  //Kawa always (maybe temperature, etc. every second try)

  //Suzi only on RPM Requests
  if (IsSuzuki()) {
    //if last storage update is older than 1,5sec, read immediately
    if(millis() - lastDataUpdate > K_Keepalive)
      return true;
    //Current Data
    if (data.SID == 0x01)
      return data.PID == 0x0C;

    //Freezed Frames -> Allways read fresh! Time is not important here...
    //    if (data.SID == 0x02)
    //      return data.PID == 0x0C;
  }
  return true;
}

void GetValuesFromBtRequest() {
  //Repeat last command (no data, just CR/LF)
  if (btBufferCounter == 0) {
    //Already done by last request:
    msgLife.SkipSidTranslation = true;
    msgLife.SkipPidTranslation = true;
    return;
  }

  data.TranslatedSID = 0xFF;
  data.TranslatedPID = 0xFF;

  data.SID = btBuffer[0];
  if (btBufferCounter >= 2)
    data.PID = btBuffer[1];
  else
    data.PID = 0xFF;
}

bool TranslateBtRequestSid() {
  if (rawMode)
    return true;

  switch (data.SID) {
    case 0x01: //Show current data
      data.TranslatedSID = 0x21;
      break;
    case 0x02: //Show freeze frame data    
      data.TranslatedSID = 0x12;
      break;
    case 0x03: //Show stored Diagnostic Trouble Codes    
      //0x13 0x02
      data.TranslatedSID = 0x13;
      msgLife.SkipPidTranslation = true;
      break;
    case 0x04: //Clear Diagnostic Trouble Codes and stored values
      data.TranslatedSID = 0x14;
      break;
    case 0x05: //Test results, oxygen sensor monitoring (non CAN only) [lambda sonde]
      return false;
    case 0x06: //Test results, other component/system monitoring (Test results, oxygen sensor monitoring for CAN only)
      return false;
    case 0x07: //Show pending Diagnostic Trouble Codes (detected during current or last driving cycle)
      data.TranslatedSID = 0x13;
      msgLife.SkipPidTranslation = true;
      break;
    case 0x08: //Control operation of on-board component/system
      if (IsSuzuki())
        data.TranslatedSID = 0xA5;
      if (IsKawasaki())
        data.TranslatedSID = 0x31;//31 start, 32 stop
      break;
    case 0x09: //Request vehicle information
      //        0x02: Vehicle Identification Number
      //        0x04: Calibration Identifications
      //        0x06: Calibration Verification Number
      //        0x08: In-use Performance Tracking (Spark)
      //        0x0A: ECU Name
      //        0x0B: In-use Performance Tracking (Compression)
      //        0x0D: Engine Serial Number
      //        0x0F: Exhaust Regulation Or Type Approval Number
      data.TranslatedSID = 0x1A; //???? 1A is ECU Id...
      msgLife.SkipPidTranslation = true;
      break;
    case 0x0A: //Permanent Diagnostic Trouble Codes (DTCs) (Cleared DTCs)
      data.TranslatedSID = 0x13;
      msgLife.SkipPidTranslation = true;
      break;
    //Already SEA J2190 Requests
    case 0x13: //Read Diagnostic Trouble Codes
    case 0x17: //Read Status Of Diagnostic Trouble Codes
    case 0x18: //Read Diagnostic Trouble Codes By Status
    case 0x19: //?!?
      data.TranslatedSID = data.SID;
      data.TranslatedPID = data.PID;
      //add optional, additional data:
      for (uint8_t i = 2; i < btBufferCounter; i++)
        ecuBufferOutTmp[ecuBufferOutTmpCounter++] = btBuffer[i];
      msgLife.SkipPidTranslation = true;
      msgLife.ResponseToBtFromStorage = false;
      msgLife.ResponseToBtFromEcu = true;
      break;
    default:
      SetError(ERROR_Unknown_SID);
      data.TranslatedSID = data.SID;
      return false;
      break;
  }
  return true;
}

bool TranslateBtRequestPid() {
  if (rawMode)
    return true;

  if (IsKawasaki())
    return TranslateKawaDataPid();
  if (IsSuzuki())
    return TranslateSuzukiDataPid();
  if (IsHonda())
    return TranslateHondaDataPid();

  return false;
}

bool FakeBtResponse() {
  if (rawMode)
    return true;

  uint8_t resp[10];
  uint8_t respLen = 0;

  resp[respLen++] = data.SID + 0x40;
  resp[respLen++] = data.PID;

  //Livedata for all, Freezed Frames differ for Suzuki
  if (data.SID == 0x01 || (data.SID == 0x02 && IsKawasaki())) {
    switch (data.PID) {
      //PID Lists
      case 0x00:
        //Freeze DTC | Engine Load (For Gear) | Engine Coolant Temp | Intake Manifold Pressure | RPM | Speed | Advance | Intake Air Temp | MAF (nope) | Throttle | Run time since start | PIDs 33-64
        resp[respLen++] = B01011000;//0x58;
        resp[respLen++] = B00111110;//0x3E; //Removed MAF
        resp[respLen++] = B10000000;//0x80;
        resp[respLen++] = B00010011;//0x13;
        break;
      case 0x20:
        //Absolut Barometric
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00100000;//0x20;
        resp[respLen++] = B00000001;//0x01;
        break;
      case 0x40:
        //Internal Control Voltage | Relative throttle position in % | Fuel Type
        resp[respLen++] = B01001000;//0x48;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B10000000;//0x80;
        resp[respLen++] = B00000001;//0x01;
        break;
      case 0x60:
        //Engine Runtime
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000011;//0x03;
        break;
      case 0x80:
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000001;//0x01;
        break;
      case 0xA0:
        //Transmission Gear
        resp[respLen++] = B00010000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x01;
        break;
      case 0xC0:
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        break;
      //PID constants
      case 0x1C: //OBD compliant
        resp[respLen++] = 0x05; //Non OBD compliant
        break;
      case 0x4F: //Maximum values Fuel, Oxy, Voltage 4 Byte
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
        break;
      case 0x50: //Maximum value for AirFlow Rate (4Byte) 0-25550
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
        break;
      case 0x51: //Fuel Type
        resp[respLen++] = 0x01; //Gasoline
        break;
      case 0x1F: //Runtime since start
      {
        uint16_t runtime = lastKlineResponse / 1000;
        resp[respLen++] = highByte(runtime);
        resp[respLen++] = lowByte(runtime);
        break;
      }
      default:
        return false;
    }

    SendBtResponse(resp, respLen);
    msgLife.SkipSidTranslation = true;
    msgLife.SkipPidTranslation = true;
    msgLife.RequestToEcu = false;
    msgLife.ResponseToBt = false;
    return true;
  }

  if (data.SID == 0x02 && IsSuzuki()) {
    switch (data.PID) {
      //PID Lists
      case 0x00:
        //RPM | Throttle | Manifold Pressure | Enginge Coolant | Gear (Engine Load) | Sec Throttle
        resp[respLen++] = B00011000;//0x18;
        resp[respLen++] = B00110000;//0x30;
        resp[respLen++] = B10000000;//0x80;
        resp[respLen++] = B00000001;//0x01;
        break;
      case 0x20:
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000001;//0x01;
        break;
      case 0x40:
        resp[respLen++] = B00001000;//0x08;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        break;
      case 0x60:
      case 0x80:
      case 0xA0:
      case 0xC0:
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        break;
      default:
        return false;
    }
    SendBtResponse(resp, respLen);
    msgLife.SkipSidTranslation = true;
    msgLife.SkipPidTranslation = true;
    msgLife.RequestToEcu = false;
    msgLife.ResponseToBt = false;
    return true;
  }

  //Request vehicle information
  if (data.SID == 0x09) {
    switch (data.PID) {
      //PID Lists
      case 0x00: //Supported PID´s
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B01000000;//0x40; 0A supported
        resp[respLen++] = B00000000;//0x00;
        resp[respLen++] = B00000000;//0x00;
        break;
      case 0x02: //Vehicle Identification Number
      case 0x04: //Calibration Identifications
      case 0x06: //Calibration Verification Number
      case 0x08: //In-use Performance Tracking (Spark)
      case 0x0B: //In-use Performance Tracking (Compression)
      case 0x0D: //Engine Serial Number
      case 0x0F: //Exhaust Regulation Or Type Approval Number
        resp[respLen++] = 0x00; //Nothing...
        break;
      case 0x0A: //ECU Name
        return false; //Send Request do ECU
      default:
        return false;
    }
    SendBtResponse(resp, respLen);
    msgLife.SkipSidTranslation = true;
    msgLife.SkipPidTranslation = true;
    msgLife.RequestToEcu = false;
    msgLife.ResponseToBt = false;
    return true;
  }
  return false;
}

void ProcessSpecificCalls() {
  //Read DTC (Stored, Pending, Permanent)
  //SID 0x03
  if (data.TranslatedSID == 0x13) {    
    DTC();    
    msgLife.RequestToEcu = false;
    msgLife.ResponseToBtFromStorage = true;
    msgLife.ResponseToBtFromEcu = false;
  }

  //01 02: Freezed DTC
  if (data.SID == 0x01 && data.PID == 0x02) {
    FreezedFrames();
    msgLife.RequestToEcu = false;
    msgLife.ResponseToBtFromStorage = true;
  }

  //ECU ID
  //Request & Translate Response (No storage)
  if (data.TranslatedSID == 0x1A) {
    EcuId();
    msgLife.ResponseToBtFromStorage = false;
    msgLife.ResponseToBtFromEcu = true;
  }

  //Control operation of on-board component/system (Test mode)
  if (data.SID == 0x08) {
    //ToDo: Do specific bike stuff for testing
  }
}

bool RespondFromStorage() {
  uint8_t resp[13];
  uint8_t respLen = 0;

  resp[respLen++] = data.SID + 0x40;
  if (data.PID != 0xFF)
    resp[respLen++] = data.PID;

  //Current data || Freezed Frame data
  if (data.SID == 0x01 || data.SID == 0x02)
    switch (data.PID) {
      case 0x02:
        RespondFreezedFrames(resp, respLen);
        break;
      case 0x04: //Engine load
        resp[respLen++] = data.EngineLoad;
        break;      
      case 0x05:
        resp[respLen++] = data.EngineCoolant;
        break;
      case 0x0B:
        resp[respLen++] = data.IMAP;
        break;
      case 0x0C:
        resp[respLen++] = data.RPM[0];
        resp[respLen++] = data.RPM[1];
        break;
      case 0x0D:
        resp[respLen++] = data.Speed;
        break;
      case 0x0E:
        resp[respLen++] = data.TimingAdvance;
        break;
      case 0x0F:
        resp[respLen++] = data.IntakeTemperature;
        break;
      case 0x10:        
        //ToDo: Calculate Value -> mmHg (Pressure) to MFA (Mass flow Rate) not possible!
        //MAF not exits on Kawa or Suzuki, Take Intake Air pressure instead:
        resp[respLen++] = data.IMAP;
        break;
      case 0x11:
        resp[respLen++] = data.TPS;
        break;      
      case 0x33:
        resp[respLen++] = data.ABAP;
        break;
      case 0x42:
        resp[respLen++] = data.ControlModuleVoltage[0];
        resp[respLen++] = data.ControlModuleVoltage[1];
        break;
      case 0x45:
        resp[respLen++] = data.SecTPS;
        break;
      case 0xA4: //Transmission current gear        
        //resp[respLen++] = 0xA1; //A1: Gear supported, A2 Ratio supported (A0 unsupported)
        resp[respLen++] = (data.TransmissionGear << 4) +1; //Seems to shift gear >>4 aside and add 1 for "supported"        
        break;      
      case 0x7F:        
        resp[respLen++] = 0x01; //Runtime, 02 Idle Time, 04 PTO Time
        resp[respLen++] = 0x00; //4 byte
        resp[respLen++] = data.EngineRuntime[0];
        resp[respLen++] = data.EngineRuntime[1];
        resp[respLen++] = data.EngineRuntime[2];
        break;
      default:
        return false;
    }

  //Freezed Frames SID 01 PID 02, not 03 or 07...
  //Stored DTC
  //if (data.SID == 0x03)
    //RespondFreezedFrames(resp, respLen);
  //Current/Pending DTC
  //if (data.SID == 0x07) {
  if (data.SID == 0x03 || data.SID == 0x07) {
    //DTC 1 & 2
    for (uint8_t i = 0; i < 2; i++) {
      if (data.DTC[i][0] != 0xFF) {
        resp[respLen++] = data.DTC[i][0];
        resp[respLen++] = data.DTC[i][1];
      }
      else {
        resp[respLen++] = 0x00;
        resp[respLen++] = 0x00;
      }
    }
  }
  //Permanent DTC (We might identify that by state [Suzuki only])
  if (data.SID == 0x0A)
    for (uint8_t i = 0; i < 6; i++)
      resp[respLen++] = 0x00;

  SendBtResponse(resp, respLen);
  return true;
}

void RespondFreezedFrames(uint8_t *resp, uint8_t &respLen) {
  for (uint8_t f = 0; f < 3; f++) {
    if (data.FreezedFrames[f][0] != 0xFF) {
      resp[respLen++] = data.FreezedFrames[f][0];
      resp[respLen++] = data.FreezedFrames[f][1];
    }
  }
}

void RespondFromEcu() {
  //uint8_t resp[13];
  uint8_t resp[BUFFER_SIZE];
  uint8_t respLen = 0;

  //Length is always after Format
  uint8_t len = ecuBufferIn[3];
  //start after length
  uint8_t start = 4;

  //if contains SID and PID, replace with proper translated ones.
  if (ecuBufferIn[start] == data.TranslatedSID  + 0x40) {
    resp[respLen++] = data.SID + 0x40;
    start++;
    len--;
  }
  //Skip PID, when empty
  if (data.TranslatedPID != 0xFF && ecuBufferIn[start] == data.TranslatedPID) {
    resp[respLen++] = data.PID;
    start++;
    len--;
  }

  //len 11
  //start 5
  //ECU ID:
  //80 F1 11 0B 5A 32 31 31 37 35 2D 30 32 37 32 DF
  // 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  //         Len SID
  for (uint8_t i = start; i < start + len; i++) {
    //Ensure no overflow
    if (i < ecuBufferInCounter)
      resp[respLen++] = ecuBufferIn[i];
  }
  SendBtResponse(resp, respLen);
}

bool CheckForError() {
  uint8_t pos = 3;//Format, Receiver, Sender, ERROR, SID, ErrorNo
  if (ecuBufferIn[0] == 0x80)
    pos = 4;//Format, Receiver, Sender, Length, ERROR, SID, ErrorNo

  if (ecuBufferIn[pos] != 0x7F)
    return false;
  pos += 2;
  SendBtError(ecuBufferIn[pos]);
  SetError(ERROR_7F_Response, ecuBufferIn[pos]);
  return true;
}

void SendBtError(uint8_t errorId) {
  uint8_t sid = data.SID;
  uint8_t data[3] = { 0x7F, sid, errorId};
  SendBtResponse(data, 3);
}

void SendBtResponse(uint8_t *msg, uint8_t len) {
  if (!msgLife.ResponseToBt)
    return;
  
  uint8_t cks = 0;  

  if (header) {
    //Format
    if (customFormat == 0xFF)
      PrintBtResponse(cks, 0x80);
    else
      PrintBtResponse(cks, customFormat);    

    //Receiver
    PrintBtResponse(cks, myAddress);    

    //Sender
    PrintBtResponse(cks, ecuAddress);    

    //Length
    PrintBtResponse(cks, len);    
  }

  //Data
  for (uint8_t i = 0; i < len; i++) {
    PrintBtResponse(cks, msg[i], (i < len - 1));
  }

  //Checksum
  if (header) {
    PrintSpace();
    PrintHex(cks);
  }
  PrintFooter(false);
}
void PrintBtResponse(uint8_t& cks, uint8_t val){
   PrintBtResponse(cks, val, true);
}
void PrintBtResponse(uint8_t& cks, uint8_t val, bool space){
  cks += val;
  PrintHex(val);
  if(space)
    PrintSpace();
}

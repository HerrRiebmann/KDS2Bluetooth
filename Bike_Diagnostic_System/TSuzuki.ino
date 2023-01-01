void TransferSuzukiToStorage() {
  uint32_t value = 0;

  //No of DTC
  data.ErrorCount = ecuBufferIn[6];

  //Freezed DTC 1
  data.FreezedFrames[0][0] = ecuBufferIn[7] == 0xFF ? 0x00 : ecuBufferIn[7];
  data.FreezedFrames[0][1] = ecuBufferIn[8] == 0xFF ? 0x00 : ecuBufferIn[8];
  //State
  //data.FreezedFrames[0][2] = ecuBufferIn[9];
  //Freezed DTC 2
  data.FreezedFrames[1][0] = ecuBufferIn[10] == 0xFF ? 0x00 : ecuBufferIn[10];
  data.FreezedFrames[1][1] = ecuBufferIn[11] == 0xFF ? 0x00 : ecuBufferIn[11];
  //State
  //data.FreezedFrames[1][2] = ecuBufferIn[12];

  //Faked Gear - OK
  data.EngineLoad = map(ecuBufferIn[26], 0, 6, 0, 15);
  //Real Gear
  data.TransmissionGear = ecuBufferIn[26];

  //Engine coolant Temp - OK
  if (ecuBufferIn[21] != 0x00)
    data.EngineCoolant = ((uint32_t)ecuBufferIn[21] * 160 / Factor) + 10;

  //Intake manifold absolute pressure (kPa) - value seems right, calc seems hardly guessed
  if (ecuBufferIn[20] != 0x00)
    //value = (uint32_t)ecuBufferIn[16] * 190*5/Factor
    //From c# project A * 166.7f / 255f - 20f
    //kPa (OBD compliant)
    data.IMAP = ecuBufferIn[20] * 166.7 / Factor - 20;

  //Speed - Not available on GSX-R 600 - K6
  if (ecuBufferIn[16] == 0xFF)
    data.Speed = 0;
  else
    data.Speed = ecuBufferIn[16];  //Should be /2 but then it won´t fit to >250-300 kmh

  //RPM - OK
  if (ecuBufferIn[17] + ecuBufferIn[18] == 0)
    value = 0;
  else    
    value = (((uint32_t)ecuBufferIn[17] << 8) + ecuBufferIn[18]) * 100 / Factor * 4;
  data.RPM[0] = highByte(value);
  data.RPM[1] = lowByte(value);

  //Intake Air Temp - OK
  if (ecuBufferIn[22] != 0x00)    
    data.IntakeTemperature = (uint32_t)ecuBufferIn[22] * 160 / Factor + 10;

  //Throttle pos. - Max value wrong
  if (throttlePosMaximum < ecuBufferIn[19]) {
    throttlePosMaximum = ecuBufferIn[19];
    SetThrottleValues();
  }
  if (throttlePosMinimum > ecuBufferIn[19]) {
    throttlePosMinimum = ecuBufferIn[19];
    SetThrottleValues();
  }
  data.TPS = map(ecuBufferIn[19], throttlePosMinimum, throttlePosMaximum, 0, 255);

  //Absolute barometric pressure (kPa)
  if (ecuBufferIn[23] != 0x00)
    //Like IMP
    data.ABAP = ecuBufferIn[23] * 166.7 / Factor - 20;

  //Control module voltage - Measure!
  if (ecuBufferIn[24] != 0x00) {
    value = (uint32_t)ecuBufferIn[24] * 20 * 1000 / Factor;
    data.ControlModuleVoltage[0] = highByte(value);
    data.ControlModuleVoltage[1] = lowByte(value);
  }

  //Relative Throttle Pos
  if (subThrottleMaximum < ecuBufferIn[46]) {
    subThrottleMaximum = ecuBufferIn[46];
    SetThrottleValues();
  }
  if (subThrottleMinimum > ecuBufferIn[46]) {
    subThrottleMinimum = ecuBufferIn[46];
    SetThrottleValues();
  }
  data.SecTPS = map(ecuBufferIn[46], subThrottleMinimum, subThrottleMaximum, 0, 255);

  //If automatic gear detection, overwrite EngingeLoad
  if (autoGear)
    data.EngineLoad = CalcGearToEngineLoad(GetCurrentGear());

  //Timing Advance (Cylinder 1)
  if (ecuBufferIn[41] != 0x00 && ecuBufferIn[41] != 0xff) {    
    data.TimingAdvance = ecuBufferIn[41] + 78; //64 * 2 - 50
  }
}

void TransferSuzukiFFToStorage() {
  uint32_t value = 0;

  //Faked Gear - OK
  data.EngineLoad = CalcGearToEngineLoad(ecuBufferIn[15]);

  //Engine coolant Temp - OK
  if (ecuBufferIn[14] != 0x00)
    data.EngineCoolant = ((uint32_t)ecuBufferIn[14] * 160 / Factor) + 10;
  else
    data.EngineCoolant = 0x00;

  //Intake manifold absolute pressure (kPa) - value seems right, calc seems hardly guessed
  if (ecuBufferIn[13] != 0x00)
    data.IMAP = ecuBufferIn[13] * 166.7 / Factor - 20;

  //RPM
  value = (((uint32_t)ecuBufferIn[10] << 8) + ecuBufferIn[11]) * 100 / Factor * 4;
  data.RPM[0] = highByte(value);
  data.RPM[1] = lowByte(value);

  //TPS
  data.TPS = map(ecuBufferIn[12], throttlePosMinimum, throttlePosMaximum, 0, 255);
  //STPS
  data.SecTPS = map(ecuBufferIn[16], subThrottleMinimum, subThrottleMaximum, 0, 255);
}

void SuzukiGetFreezedFrame(uint8_t frame) {
  //Error1: 40-44 (Pre, Point, Post, Fixed, Min, Max)
  //Error2: 50-54
  //80 F1 12 12 61 41 16 55 A0 FF 00 00 39 BA 5B 00 09 FF FF FF FF FF 93

  //80 12 F1 02 21 41 E7
  uint8_t reqLen = 2;
  uint8_t req[2];
  req[0] = 0x21;

  //Fixed Detectpoint
  if (frame == 0x01)
    req[1] = 0x41;
  else
    req[1] = 0x51;

  SendToKline(req, reqLen);
  ReceiveKlineResponse();

  //Then store response values to data
  TransferSuzukiFFToStorage();
}

void SuzukiSetDTC() {
  //No of Errors:
  //80 12 F1 04 18 00 00 00 9F
  //80 F1 12 0E 58 04 03 51 A0 03 53 A0 03 54 A0 16 55 A0 D9
  //
  uint8_t reqLen = 4;
  uint8_t req[4];
  req[0] = 0x18;
  req[1] = 0x00;
  req[2] = 0x00;
  req[3] = 0x00;

  SendToKline(req, reqLen);
  ReceiveKlineResponse();

  for (int i = 0; i < ecuBufferIn[5]; i++) {
    //0, 1, 2, 3
    //0, 3, 6, 9
    data.DTC[i][0] = ecuBufferIn[i * 3 + 6];
    data.DTC[i][1] = ecuBufferIn[i * 3 + 7];
    //State:
    data.DTC[i][2] = ecuBufferIn[i * 3 + 8];
  }
}

void SuzukiSetAllData() {
  uint8_t reqLen = 2;
  uint8_t req[2];
  req[0] = 0x21;
  req[1] = 0x08;
  SendToKline(req, reqLen);
  ReceiveKlineResponse();
  TransferSuzukiToStorage();
}

void SuzukiClearDTC() {
  //80 12 F1 03 14 00 00 9A
  uint8_t reqLen = 3;
  uint8_t req[5];
  req[0] = 0x14;  //Clear Diagnostic Information
  req[1] = 0x00;
  req[2] = 0x00;
  SendToKline(req, reqLen);
  ReceiveKlineResponse();
}

bool TranslateSuzukiDataPid() {
  //Freezed Frames
  if (data.TranslatedSID == 0x12) {
    //Only supported: RPM | Throttle | Manifold Pressure | Engine Coolant | Gear (Engine Load) | Sec Throttle
    switch (data.PID) {
      case 0x04:  //Engine Load (Gear)
      case 0x05:  //Engine Coolant
      case 0x0C:  //RPM
      case 0x0B:  //Intake Manifold absolute
      case 0x11:  //Throttle Position
      case 0x45:  //Sec. Throttle
        SuzukiGetFreezedFrame(btBuffer[2]);
        msgLife.RequestToEcu = false;
        msgLife.ResponseToBtFromStorage = true;
        return true;
    }
  }
  //Clear Diagnostic Trouble Codes
  if (data.TranslatedSID == 0x14) {
    data.TranslatedPID = 0x00;
    ecuBufferOutTmp[ecuBufferOutTmpCounter++] = 0x00;
    msgLife.ResponseToBtFromStorage = false;
    msgLife.ResponseToBtFromEcu = true;
    return true;
  }
  //Live data 0x21
  if (data.TranslatedSID == 0x21) {
    switch (btBuffer[1]) {
      case 0x02:  //Read DiagnosticTroubleCodes
      case 0x04:  //Engine Load 1 Byte - OK
      case 0x05:  //Engine Coolant Temperature 1 Byte - OK
      case 0x0B:  //Intake Manifold Air Pressure 1 Byte
      case 0x0C:  //Engine RPM 2Byte - OK
      case 0x0D:  //Speed 1 Byte - OK
      case 0x0E:  //Advance 1 Byte - OK
      case 0x0F:  //Intake Air Temperature 1 Byte - OK
      case 0x10:  //MAF 2 Byte - OK [Not supported by garmin]
      case 0x11:  //Throttle Position 1 Byte - OK
      case 0x1C:  //OBD standard this vehicle conforms to 1 Byte
      case 0x1E:  //Wildcard for Gear
      case 0x33:  //Barometric Pressure 1 Byte
      case 0x42:  //Internal Control Voltage 2 Byte
      case 0x45:  //Relative Throttle Position 1 Byte
      case 0x4F:  //Maximum values Fuel, Oxy, Voltage 4 Byte
      case 0x50:  //Maximum values Air flow rate 4 Byte
      case 0x51:  //Fuel Type 1 Byte
      case 0xA4:  //Transmission actual gear
        data.TranslatedPID = 0x08;
        return true;
      case 0x7F:  //TotalOperatingHours / Engine Runtime
      default:
        SetError(ERROR_Unknown_PID);
        data.TranslatedPID = 0x00;
        return false;
    }
  }
  return false;
}

void TransferKawaToStorage() {
  unsigned int value = 0;
  //Reset PID on Freezed Frames (Length + 3)
  if (data.TranslatedSID == 0x12)
    data.TranslatedPID = ecuBufferIn[ecuBufferIn[3] + 3];
  switch (data.TranslatedPID) {
    case 0x04:  //Throttle Position Sensor
      //201 = 0% = idle, 892 = 100%
      value = CalculateValue(Factor, 0);

      if (throttlePosMinimum > value) {
        throttlePosMinimum = value;
        SetThrottleValues();
      }

      if (value > throttlePosMaximum) {
        throttlePosMaximum = value;
        SetThrottleValues();
      }
      //((Value-Minimum) *100) / (Maximum - Minimum) = %
      //OBD II Calculation: (100/255) * A [Backwards]: 100% = 255
      if (value > throttlePosMinimum)
        value = map(value, throttlePosMinimum, throttlePosMaximum, 0, 255);
      else
        value = 0x00;
      data.TPS = value;
      break;
    case 0x05:  //Airpressure (Inlet manifold)
      value = CalculateValue(190.0 * 5.0 / 256);
      //OBD mm Hg to kPa
      value = map(value, 0, 800, 0, 255);
      data.IMAP = value;
      break;
    case 0x08:  //Athmospheric Manifold Pressure in kPa
      value = CalculateValue(190.0 * 5.0 / Factor);
      //OBD mm Hg to kPa
      value = map(value, 0, 800, 0, 255);
      data.ABAP = value;
      break;
    case 0x06:  //Engine coolant Temp
      //OBD C° + 40
      value = CalculateValue(160);
      value += 10;
      data.EngineCoolant = value;
      break;
    case 0x07:  //Intake Air Temp
      //value = CalculateValue(160, 30);
      //OBD C° + 40
      value = CalculateValue(160);
      value += 10;
      data.IntakeTemperature = value;
      break;
    case 0x09:  //RPM
      value = CalculateValue(100);
      //OBD: (256A + B) / 4
      value *= 4;
      data.RPM[0] = highByte(value);
      data.RPM[1] = lowByte(value);
      break;
    case 0x0A:  //Battery Voltage
      value = CalculateValue(20);
      //OBD II
      //((A*256)+B)/1000
      //2 Byte
      value *= 1000;
      //Test: (Dont divide)
      {
        uint32_t bigInt = CalculateValue(Factor);
        bigInt *= 1000;
        bigInt *= 20;
        bigInt /= Factor;
        value = bigInt;
      }
      data.ControlModuleVoltage[0] = highByte(value);
      data.ControlModuleVoltage[1] = lowByte(value);
      break;
    case 0x0B:  //Gear
      value = CalculateValue(Factor);
      data.TransmissionGear = value;
      value = CalcGearToEngineLoad(value);
      data.EngineLoad = value;
      break;
    case 0x0C:  //Speed
      value = CalculateValue(Factor) / 2;
      data.Speed = value;
      break;
    case 0x44:  //Total operating seconds (13 bytes)
      data.EngineRuntime[0] = ecuBufferIn[7];
      data.EngineRuntime[1] = ecuBufferIn[8];
      data.EngineRuntime[2] = ecuBufferIn[9];
      break;
    case 0x56:  //Failure Count
      value = CalculateValue(Factor);
      data.ErrorCount = value;
      break;
    case 0x5B:  //Sub Throttle valve Sensor
      //Voltage
      //value = CalculateValue(5, 0);
      //81 = 0% = idle, (?)189 = 100% //will be adjusted dynamically
      value = CalculateValue(Factor);
      if (value < subThrottleMinimum) {
        subThrottleMinimum = value;
        SetThrottleValues();
      }
      if (value > subThrottleMaximum) {
        subThrottleMaximum = value;
        SetThrottleValues();
      }
      //((Value-Minimum) *100) / (Maximum - Minimum) = %
      //OBD: A*100/255
      value = map(value, subThrottleMinimum, subThrottleMaximum, 0, 255);
      data.SecTPS = value;
      break;
    case 0x5F:  //Internal Voltage
      value = CalculateValue(5);
      //OBD II
      //((A*256)+B)/1000
      //2 Byte
      value *= 1000;
      {
        uint32_t bigInt = CalculateValue(Factor);
        bigInt *= 1000;
        bigInt *= 5;
        bigInt /= Factor;
        value = bigInt;
      }
      data.ControlModuleVoltage[0] = highByte(value);
      data.ControlModuleVoltage[1] = lowByte(value);
      break;
    case 0x66:  //Exhaust operating angle
      value = CalculateValue(5);
      break;
    default:
      break;
  }
}

void KawaSetFreezedFrames() {
  uint8_t reqLen = 2;
  uint8_t req[5];
  req[0] = 0x21;
  req[1] = 0x56; // Counter

  SendToKline(req, reqLen);
  ReceiveKlineResponse();
  uint8_t failCounter = ecuBufferIn[6];

  //Error:
  if (ecuBufferIn[4] == 0x7F)
    failCounter = 0;

  req[0] = 0x12;
  //group of DTC
  req[2] = 0x01;  // 00 All data, 01 by local indentifier, 02 by common indentifier, ect.
  reqLen = 4;

  for (int i = 1; i <= failCounter; i++) {
    req[1] = i;  //Number of freezed Frame
    //PID: Failure Record:
    req[3] = 0x54;
    SendToKline(req, reqLen);
    ReceiveKlineResponse();  //01 15 07 01 54 | 02 10 02 01 54 | 03 10 01 01 54
    data.FreezedFrames[i - 1][0] = ecuBufferIn[6];
    data.FreezedFrames[i - 1][1] = ecuBufferIn[7];
  }
}

void KawaSetDTC() {
  uint8_t reqLen = 2;
  uint8_t req[2];
  req[0] = 0x13;

  req[1] = 0x01;
  SendToKline(req, reqLen);
  ReceiveKlineResponse();
  //No Error stored:
  //80 F1 11 02 53 00 D7
  //Current Error
  //80 F1 11 04 53 01 11 04 EF
  if (ecuBufferIn[5] == 0x01) {
    data.DTC[0][0] = ecuBufferIn[6];
    data.DTC[0][1] = ecuBufferIn[7];
  } else {
    data.DTC[0][0] = 0x00;
    data.DTC[0][1] = 0x00;
  }

  req[1] = 0x02;
  SendToKline(req, reqLen);
  ReceiveKlineResponse();
  //80 F1 11 04 53 01 15 08 F7
  if (ecuBufferIn[5] == 0x01) {
    data.DTC[1][0] = ecuBufferIn[6];
    data.DTC[1][1] = ecuBufferIn[7];
  } else {
    //Empty
    data.DTC[1][0] = 0x00;
    data.DTC[1][1] = 0x00;
  }
}

void KawaClearDTC() {
  //80 11 F1 02 14 01 99
  uint8_t reqLen = 2;
  uint8_t req[2];
  req[0] = 0x14;  //Clear Diagnostic Information
  req[1] = 0x01;
  SendToKline(req, reqLen);
  ReceiveKlineResponse();
  //80 F1 11 01 54 D7
}

void KawaGetVoltage() {
  data.TranslatedSID = 0x21;
  data.TranslatedPID = 0x0A;
  SendToKline();
  ReceiveKlineResponse();
  TransferKawaToStorage();
}

bool TranslateKawaDataPid() {
  //Freezed Frames
  if (data.TranslatedSID == 0x12) {
    //Translate PID
    if (!TranslateKawaPid(btBuffer[1]))
      return false;
    ecuBufferOutTmp[1] = data.TranslatedPID;  //Set PID to higher position (after FF no)
    data.TranslatedPID = btBuffer[2];         //No of Freezed Frame (Overwrite to reuse)
    ecuBufferOutTmp[0] = 0x01;                // 00 All data, 01 by local indentifier, 02 by common indentifier, ect.
    ecuBufferOutTmpCounter = 2;
    return true;
  }

  //Clear Diagnostic Trouble Codes
  if (data.TranslatedSID == 0x14) {
    data.TranslatedPID = 0x01;
    msgLife.ResponseToBtFromStorage = false;
    msgLife.ResponseToBtFromEcu = true;
    return true;
  }

  //Live data 0x21
  if (data.TranslatedSID == 0x21)
    return TranslateKawaPid(data.PID);
  return false;
}

bool TranslateKawaPid(uint8_t pid) {
  switch (pid) {
    case 0x02:  //Read DiagnosticTroubleCodes from Freezed Frames
      data.TranslatedPID = 0x02;
      break;
    case 0x04:  //Engine Load 1 Byte - OK
    case 0xA4:  //Transmission actual gear
      //Throttle Opening & Engine Speed (RPM)
      //Or Inlet Air vacuum & RPM

      //If automatic gear detection, overwrite EngingeLoad
      if (autoGear) {
        data.TransmissionGear = GetCurrentGear();
        data.EngineLoad = CalcGearToEngineLoad(data.TransmissionGear);
        msgLife.RequestToEcu = false;
        msgLife.ResponseToBtFromStorage = true;
      } else
        data.TranslatedPID = 0x0B;
      break;
    case 0x05:  //Engine Coolant Temperature 1 Byte - OK
      data.TranslatedPID = 0x06;
      break;
    case 0x0B:                    //Intake Manifold Air Pressure 1 Byte
      data.TranslatedPID = 0x05;  //Inlet Air Pressure
      break;
    case 0x0C:  //Engine RPM 2Byte - OK
      data.TranslatedPID = 0x09;
      break;
    case 0x0D:  //Speed 1 Byte - OK
      data.TranslatedPID = 0x0C;
      break;
    case 0x0F:  //Intake Air Temperature 1 Byte - OK
      data.TranslatedPID = 0x07;
      break;
    case 0x10:                    //MAF 2 Byte - OK [Not supported by garmin]
      data.TranslatedPID = 0x05;  //MAF is nearly the same like Intake Air Pressure... nope!
      break;
    case 0x11:  //Throttle Position 1 Byte - OK
      data.TranslatedPID = 0x04;
      break;
    case 0x1C:  //OBD standard this vehicle conforms to 1 Byte
      //Will be faked
      break;
    case 0x33:                    //Barometric Pressure 1 Byte
      data.TranslatedPID = 0x08;  //Atmospheric Pressure
      break;
    case 0x42:  //Internal Control Voltage 2 Byte
      //Voltage Internal C2
      data.TranslatedPID = 0x5F;
      //Battery Voltage (fetched by AT-RV)
      //data.TranslatedPID = 0x0A;
      break;
    case 0x45:  //Relative Throttle Position 1 Byte
      data.TranslatedPID = 0x5B;
      break;
    //case 0x1C: ///OBD compliant
    case 0x4F:  //Maximum values Fuel, Oxy, Voltage 4 Byte
    case 0x50:  //Maximum values Air flow rate 4 Byte
    case 0x51:  //Fuel Type 1 Byte
      //Will be covered by FakeBtResponse()
      break;
    case 0x7F:  //TotalOperatingHours / Engine Runtime
      data.TranslatedPID = 0x44;
      break;
    default:
      SetError(ERROR_Unknown_PID);
      data.TranslatedPID = 0x00;
      return false;
  }
  return true;
}

void KawaAllFreezedFrames() {
  //All Freezed Frames
  static uint8_t lastPid = 0xBF;
  uint8_t reqLen = 2;
  uint8_t req[5];
  req[0] = 0x21;
  req[1] = 0x56; // Counter
  SendToKline(req, reqLen);
  ReceiveKlineResponse();
  uint8_t failCounter = ecuBufferIn[6];

  //80 11 F1 04 12 01 01 XX CKS
  reqLen = 4;
  req[0] = 0x12;  //Read Freezed Frames
  req[1] = 0x01;  //Frame 1
  req[2] = 0x01;  //Group Of DTC: 00 All data, 01 by local indentifier, 02 by common indentifier, ect.
  req[3] = 0x00;  //PID
  //ToDo: Check supported PID´s
  for (req[1] = 1; req[1] <= failCounter; req[1]++) {
    for (req[3] = 0; req[3] <= lastPid; req[3]++) {
      SendToKline(req, reqLen);
      uint8_t respLen = ReceiveKlineResponse();
      if (!debugMode) {
        BtPrint(F("<"));
        for (int i = 0; i <= respLen; i++)
          PrintHex(ecuBufferIn[i + 3]);
        BtPrintln();
      }
    }
  }
}

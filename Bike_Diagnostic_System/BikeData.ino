struct OBDData {
  uint8_t SID = 0xFF;
  uint8_t PID = 0xFF;
  uint8_t TranslatedSID = 0xFF;
  uint8_t TranslatedPID = 0xFF;

  /* Supported Data Values */
  //0x04 | 1 byte | Calculated engine load (Faked for gear)
  uint8_t EngineLoad = 0xFF;
  uint8_t TransmissionGear = 0xFF;
  //0x05 | 1 byte | Engine coolant temperature
  uint8_t EngineCoolant = 0xFF;
  //0x0B | 1 byte | Intake manifold absolute pressure
  uint8_t IMAP = 0xFF;
  //0x0C | 2 byte | Engine RPM
  uint8_t RPM[2] = { 0xFF, 0xFF };
  //0x0D | 1 byte | Vehicle Speed
  uint8_t Speed = 0xFF;
  //0x0E | 1 byte | Timing advance !!!SDS only!!!
  uint8_t TimingAdvance = 0xFF;
  //0x0F | 1 byte | Intake Air Temperature
  uint8_t IntakeTemperature = 0xFF;
  //0x10 | 2 byte | Mass air flow sensor (MAF) air flow rate
  uint8_t MAF[2] = { 0xFF, 0xFF };
  //0x11 | 1 byte | Throttle Position
  uint8_t TPS = 0xFF;
  //0x33 | 1 byte | Absolute Barometric Air Pressure
  uint8_t ABAP = 0xFF;
  //0x42 | 2 byte | Control module voltage
  uint8_t ControlModuleVoltage[2] = { 0xFF, 0xFF };
  //0x45 | 1 byte | Relative throttle position
  uint8_t SecTPS = 0xFF;
  //0x7F | 13 byte | Engine Runtime (Seconds)
  uint8_t EngineRuntime[3] = { 0xFF, 0xFF, 0xFF };

  uint8_t ErrorCount = 0xFF;

  // Diagnostic Codes //
  uint8_t DTC[2][3] = { { 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF }
  };

  // Freezed Frames - Diagnostic Codes //
  uint8_t FreezedFrames[3][2] = { { 0xFF, 0xFF }, { 0xFF, 0xFF }, { 0xFF, 0xFF } };

  // SID 01 -> Live
  // SID 02 -> Freezed Frame
  // SID 02 & PID 02 & 00 (Freezed Frame)-> DTC that caused required freeze frame data storage
  // 02 + PID + Freezed frame 0, 1, 2...
  // SID 03 -> Request Trouble Codes (3 per message)
  // 43 01 43 01 96 02 34 (03+40, DTC 1, DTC2, DTC3)

  // SID 04 -> Delete DTC
  // - PID without all. Else specific
  // SID 08 -> Request Control of On-Board System, Test or Component
  // SID 09 -> VIN
  OBDData()
    : SID(0xFF), PID(0xFF) {
  }
  void PrintDTC() {
    for (uint8_t i = 0; i < 2; i++) {
      BtPrint("DTC" + String(i) + ": ");
      BtPrintln(GetHex(DTC[i][0]) + " " + GetHex(DTC[i][1]));
      BtPrint("State: ");
      BtPrintln(GetHex(DTC[i][2]));
    }
  }
  bool Uninitialized() {
    return RPM[1] == 0xFF && EngineCoolant == 0xFF;
  }
};

OBDData data;

void TransferToStorage() {
  if (IsKawasaki())
    TransferKawaToStorage();
  if (IsSuzuki())
    TransferSuzukiToStorage();
  if (IsHonda())
    TransferHondaToStorage();
  lastDataUpdate = millis();
}

void FreezedFrames() {
  if (IsKawasaki())
    KawaSetFreezedFrames();
  if (IsSuzuki())
    if (data.Uninitialized())
      SuzukiSetAllData();
  if (IsHonda())
    HondaSetFreezedFrames();
}

void DTC() {
  if (IsKawasaki())
    KawaSetDTC();
  if (IsSuzuki())
    SuzukiSetDTC();
  if (IsHonda())
    HondaSetDTC();
}

void ClearDTC() {
  if (IsKawasaki())
    KawaClearDTC();
  if (IsSuzuki())
    SuzukiClearDTC();
  if (IsHonda())
    HondaClearDTC();
}

void SetBatteryData() {
  if (IsKawasaki())
    KawaGetVoltage();
  if (IsSuzuki())
    SuzukiSetAllData();
}

void EcuId() {
  if (IsKawasaki())
    //80 11 F1 01 1A 9D
    //Set to 0xFF to be ignored
    data.TranslatedPID = 0xFF;
  if (IsSuzuki())
    //80 12 F1 02 1A 91 30
    data.TranslatedPID = 0x91;
}

//XT Request only
void PrintDTC() {
  data.PrintDTC();
}

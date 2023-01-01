void InitializeManufacturer() {
  switch (bikeProtocol) {
    case 0:
    case 1:  //Kawasaki 1
      ecuAddress = 0x11;
      Factor = 256;
      break;
    case 2:  //Kawasaki 2 (ABS)
      ecuAddress = 0x28;
      Factor = 256;
      break;
    case 3:  //Suzuki 1
    //For later purpose
    case 4:  //Suzuki 2
      ecuAddress = 0x12;
      Factor = 256;
      break;
    case 7: //Yamaha
      ecuAddress = 0x33;
      myAddress = 0xF0;
      customFormat = 0xC2; //0xC1 for init
      Factor = 255;
      break;
    case 0xFF:  //Not yet defined
      return;
    default:
      SetError(ERROR_Unkonwn_InitProtocol);
      break;
  }
}

void SetManufacturerProtocol() {
  //Default values:
  myAddress = 0xF1;
  isoProtocol = 5;  //ISO 14230-4 KWP
  keepaliveMsg[0] = 0x3E;
  keepaliveMsg[1] = 0xFF;
  keepaliveMsg[2] = 0xFF;
  customFormat = 0xFF;
  defaultCheckSum = true;

  switch (bikeProtocol) {
    case 0:
    case 1:  //Kawasaki 1
      ecuAddress = 0x11;
      throttlePosMinimum = 201;
      throttlePosMaximum = 800;  //892;
      subThrottleMinimum = 81;
      subThrottleMaximum = 189;
      Factor = 256;
      SetTemporaryStorage(false);
      autoGear = true;
      SetAutoGear();
      break;
    case 2:  //Kawasaki 2
      ecuAddress = 0x28;
      throttlePosMinimum = 201;
      throttlePosMaximum = 892;
      subThrottleMinimum = 81;
      subThrottleMaximum = 189;
      Factor = 256;
      SetTemporaryStorage(false);
      break;
    case 3:  //Suzuki 1
    //For later usage
    case 4:  //Suzuki 2
      ecuAddress = 0x12;
      throttlePosMinimum = 0x39;  //57
      throttlePosMaximum = 0xDD;  //221 More in real...
      subThrottleMinimum = 0x1A;
      subThrottleMaximum = 0xB9;  //hardly guessed :)
      Factor = 255;
      SetTemporaryStorage(true);
      break;
    case 5:  //Honda 1
    case 6:  //Honda 2
      defaultCheckSum = false;
      break;
    case 7: //Yamaha
      //Nothing yet...
      return;
    case 0xFF:  //Not yet defined
      return;
    default:
      SetError(ERROR_Unkonwn_InitProtocol);
      break;
  }
  SetThrottleValues();
}

void ResetToDefault() {
  header = false;
  spaces = false;
  linefeed = false;
  echo = false;
  memory = false;
  adaptiveTiming = 0;
  isoProtocol = 5;
  bypassInit = false;
  //Dont write to eeprom on every ATZ
  //SetManufacturerProtocol();
  ClearBtBuffer();
  ClearKlineBuffer();
  //Custom Values:
  debugMode = false;
}

bool IsKawasaki() {
  return (bikeProtocol == 1 || bikeProtocol == 2);
}
bool IsSuzuki() {
  return (bikeProtocol == 3 || bikeProtocol == 4);
}
bool IsHonda() {
  return (bikeProtocol == 5 || bikeProtocol == 6);
}
bool IsYamaha() {
  return (bikeProtocol == 7);
}

void InitializeStorage() {
  GetOffset();
  GetErrorCounter();
  GetBtVersion();
  GetBikeProtocol();
  GetThrottleValues();
  GetRawMode();
  GetTemporaryStorage();
  GetAutoGear();
}

void GetOffset() {
  eepromOffset = EepromRead(EEPROM_Offset);
  if (eepromOffset == 0xFF)
    eepromOffset = 0;
}

void SetOffset() {
  EepromUpdate(EEPROM_Offset, eepromOffset);
}

void GetBtVersion() {
  btVersion = EepromRead(EEPROM_BTVersion);
}

void SetBtVersion() {
  EepromUpdate(EEPROM_BTVersion, btVersion);
}

void GetBikeProtocol() {
  bikeProtocol = EepromRead(EEPROM_BikeProtocol);
}

void SetBikeProtocol() {
  EepromUpdate(EEPROM_BikeProtocol, bikeProtocol);
}

void GetThrottleValues() {
  throttlePosMinimum = EepromRead(EEPROM_ThrottleMin);
  throttlePosMaximum = ((uint32_t)EepromRead(EEPROM_ThrottleMaxHigh) << 8) + EepromRead(EEPROM_ThrottleMaxLow);
  subThrottleMinimum = EepromRead(EEPROM_SubThrottleMin);
  subThrottleMaximum = EepromRead(EEPROM_SubThrottleMax);
}

void SetThrottleValues() {
  EepromUpdate(EEPROM_ThrottleMin, throttlePosMinimum);
  EepromUpdate(EEPROM_ThrottleMaxLow, lowByte(throttlePosMaximum));
  EepromUpdate(EEPROM_ThrottleMaxHigh, highByte(throttlePosMaximum));
  EepromUpdate(EEPROM_SubThrottleMin, subThrottleMinimum);
  EepromUpdate(EEPROM_SubThrottleMax, subThrottleMaximum);
}

bool GetRawMode() {
  rawMode = EepromReadBool(EEPROM_RawMode);
  return rawMode;
}

void SetRawMode(bool raw) {
  rawMode = raw;
  EepromUpdate(EEPROM_RawMode, rawMode);
}

bool GetTemporaryStorage() {
  tempStorage = EepromReadBool(EEPROM_TemporaryStorage, true);
  return tempStorage;
}

void SetTemporaryStorage(bool useTemporaryStorage) {
  tempStorage = useTemporaryStorage;
  EepromUpdate(EEPROM_TemporaryStorage, tempStorage);
}

void GetAutoGear() {
  autoGear = EepromReadBool(EEPROM_AutoGear);
}

void SetAutoGear() {
  EepromUpdate(EEPROM_AutoGear, autoGear);
}

void GetErrorCounter() {
  errorCounter = EepromRead(EEPROM_Error_Counter);
  if (errorCounter == 0xFF)
    errorCounter = 0;
}

void SetErrorCounter() {
  EepromUpdate(EEPROM_Error_Counter, errorCounter);
}

void SetError(uint8_t internalErrorCode) {
  SetError(internalErrorCode, 0xFF);
}

void SetError(uint8_t internalErrorCode, uint8_t errorCode) {
  if ((uint16_t)(errorCounter * 10 + EEPROM_Error_ID + 10) >= EEPROM.length()) {
    //Errors exeeded
    return;
  }

  if (!IsLastErrorRedundant(internalErrorCode)) {
    SetError(internalErrorCode, errorCode, errorCounter);
    errorCounter++;
    SetErrorCounter();
  }
  if (debugMode && errorCounter > 0)
    PrintErrorMessage(errorCounter - 1);
}

void SetError(uint8_t internalErrorCode, uint8_t errorCode, uint8_t errorNo) {
  uint8_t offset = errorNo * 10;
  //Last Error Code
  EepromUpdate(EEPROM_Error_ID + offset, internalErrorCode);

  if (internalErrorCode == ERROR_Unknown_AT || internalErrorCode == ERROR_Unknown_XT || internalErrorCode == ERROR_Unknown_CMD) {
    uint8_t bufferStart = internalErrorCode == ERROR_Unknown_CMD ? 0 : 2;
    //AT / XT X_
    if (btBufferCounter > bufferStart)
      EepromUpdate(EEPROM_Error_SID + offset, btBuffer[bufferStart]);
    bufferStart++;
    //AT / XT _X
    if (btBufferCounter > bufferStart)
      EepromUpdate(EEPROM_Error_PID + offset, btBuffer[bufferStart]);
  } else {
    //Last SID
    EepromUpdate(EEPROM_Error_SID + offset, data.SID);
    //Last PID
    EepromUpdate(EEPROM_Error_PID + offset, data.PID);
  }
  //Last 7F Error
  EepromUpdate(EEPROM_Error_7F + offset, errorCode);
  //Minutes since start
  uint8_t minutes = (millis() / 60000l);
  EepromUpdate(EEPROM_Error_Min + offset, minutes);
}

void ResetErrors() {
  data.SID = 0xFF;
  data.PID = 0xFF;
  for (uint8_t i = 0; i < errorCounter; i++)
    SetError(0xFF, 0xFF, i);
  errorCounter = 0;
  SetErrorCounter();
}

void ResetStorage() {
  ResetErrors();
  throttlePosMinimum = 0xFF;
  throttlePosMaximum = 0xFF;
  subThrottleMinimum = 0xFF;
  subThrottleMaximum = 0xFF;
  SetThrottleValues();
  eepromOffset = 0xFF;
  SetOffset();
  btVersion = 0xFF;
  SetBtVersion();
  bikeProtocol = 0xFF;
  SetBikeProtocol();
  SetRawMode(false);
  SetTemporaryStorage(true);
}

uint8_t EepromRead(uint8_t address) {
  return EEPROM.read(address + eepromOffset);
}

bool EepromReadBool(uint8_t address) {
  return EepromReadBool(address, false);
}
bool EepromReadBool(uint8_t address, bool defaultValue) {
  uint8_t tmpInt = EEPROM.read(address + eepromOffset);
  if (tmpInt == 0xFF)
    return defaultValue;
  return (bool)tmpInt;
}

void EepromUpdate(uint8_t address, uint8_t value) {
  EEPROM.update(address + eepromOffset, value);
}

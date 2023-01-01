void PrintErrorMessages() {
  if (errorCounter == 0 || errorCounter == 0xFF) {
    PrintNoErrors();
    return;
  }
  for (uint8_t i = 0; i < errorCounter; i++)
    PrintErrorMessage(i);
}

void PrintNoErrors() {
  BtPrint(F("No Errors!"));
}

void PrintErrorMessage(uint8_t errorNo) {  
  BtPrint("Error ");
  BtPrint(String(errorNo));

  uint16_t offset = errorNo * 10;
  //Error start adress + Counter * 10 + 10 fields
  if (offset + 10 >= EEPROM.length()) {
    BtPrint(F("OOR!"));
    return;
  }

  //Last Error Code
  uint8_t id = EepromRead(EEPROM_Error_ID + offset);
  //Last SID
  uint8_t sid = EepromRead(EEPROM_Error_SID + offset);
  //Last PID
  uint8_t pid = EepromRead(EEPROM_Error_PID + offset);
  //Last 7F Error
  uint8_t internalErrorCode = EepromRead(EEPROM_Error_7F + offset);
  //Minutes since adapterstart
  uint8_t minutes = EepromRead(EEPROM_Error_Min + offset);
  
  BtPrintln(" (" + String(minutes) + " min)");
  BtPrintln(GetErrorDescription(id));  
  if (id == ERROR_Unknown_AT || id == ERROR_Unknown_XT || id == ERROR_Unknown_CMD) {
    if (sid != 0xFF)
      BtPrint(String((char)sid));
    if (pid != 0xFF)
      BtPrint(String((char)pid));
      BtPrintln();
  }
  else {
    if (sid != 0xFF) {
      BtPrint(F("SID: "));
      BtPrintln(GetHex(sid));      
    }
    if (pid != 0xFF) {
      BtPrint(F("PID: "));
      BtPrintln(GetHex(pid));      
    }
  }
  if (internalErrorCode != 0xFF) {
    BtPrint(F("7F: "));
    BtPrintln(GetHex(internalErrorCode));    
  }  
}

bool IsLastErrorRedundant(uint8_t errorCode){
  if(errorCounter == 0)
    return false;
  uint16_t offset = (errorCounter -1) * 10;

  if (errorCode == ERROR_Unknown_AT || errorCode == ERROR_Unknown_XT || errorCode == ERROR_Unknown_CMD) {
    return errorCode == EepromRead(EEPROM_Error_ID + offset) &&
    EepromRead(EEPROM_Error_SID + offset) == btBuffer[errorCode == ERROR_Unknown_CMD ? 0 : 2] &&    
    EepromRead(EEPROM_Error_PID + offset) == btBuffer[errorCode == ERROR_Unknown_CMD ? 1 : 3];
  }  
  return errorCode == (uint8_t)EepromRead(EEPROM_Error_ID + offset) &&
         data.SID == (uint8_t)EepromRead(EEPROM_Error_SID + offset) &&
         data.PID == (uint8_t)EepromRead(EEPROM_Error_PID + offset);
}

String GetErrorDescription(uint8_t errorId) {
  switch (errorId) {
    case ERROR_StartCom: //1
      return F("StartCom");
    case ERROR_Checksum: //2
      return F("Cks");
    case ERROR_Receive_Timeout: //3
      return F("Timeout");
    case ERROR_7F_Response: //4
      return F("7F: ");
    case ERROR_Unknown_SID: //5
      return F("SID?");
    case ERROR_Unknown_PID: //6
      return F("PID?");
    case ERROR_BT_Timeout: //7
      return F("BT Timeout");
    case ERROR_Unkonwn_InitProtocol://8
      return F("No Protocol!");
    case ERROR_7F_Response_KA: //9
      return F("Keepalive 7F: ");
    case ERROR_FastInit: //10
      return F("FastInit");
    case ERROR_SlowInit: //11
      return F("SlowInit");
    case ERROR_StopCom: //12
      return F("StopCom");
    case ERROR_BT_VersionNotFound: //13
      return F("BT V.?");
    case ERROR_BT_BaudNotFound: //14
      return F("BT Baud?");
    case ERROR_BT_BaudNotChanged: //15
      return F("BT SetBaud");
    case ERROR_BT_WrongVersionInEeprom: //16
      return F("BT Wrong V.");
    case ERROR_ELMnoData: //20
      return F("NO DATA");
    case ERROR_Unknown_AT: //21
      return F("AT?");
    case ERROR_Unknown_XT: //22
      return F("XT?");
    case ERROR_Unknown_CMD: //23
      return F("CMD?");
    case ERROR_BT_Buffer_Exeeded: //24
      return F("BT Buffer");
    case ERROR_K_Buffer_Exeeded: //25
      return F("K Buffer");
    case 0:
    case 255:
    default:
      return F("-");
  }
}

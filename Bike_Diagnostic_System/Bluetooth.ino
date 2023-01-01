void InitializeBluetooth() {  
  //Uninitialized
  if (btVersion == 0xFF)
    FindBtModule();
  else
    CheckBtModule();

  //Clear outgoing buffer
  BT.flush();
  //Clear incoming buffer
  BtDataAvailable();
}

void BtWrite(const uint8_t &output) {
  BT.write(output);
}
void BtPrint(const uint8_t &i) {
  BT.print(i);
}
void BtPrint(uint8_t *msg, uint8_t len) {
  for (uint8_t i = 0; i < len; i++)
    BT.print(msg[i]);
}
void BtPrint(const String &text) {
  BT.print(text);
}
void BtPrintln() {
  BT.println();
}
void BtPrintln(const String &text) {
  BT.println(text);
}
void BtPrintln(uint8_t *msg, uint8_t len) {
  BtPrint(msg, len);
  BtPrintln();
}

void PrintBtAt() {
  if (btVersion >= 3)
    BT.println(F("AT"));
  else
    BT.print(F("AT"));
}

bool BtDataAvailable() {
  if (BT.available()) {
    //Read what´s on the buffer
    while (BT.available())
      BT.read();
    return true;
  }
  return false;
}

bool CheckBtModule() {  
  baud = BT_Baud;
  BT.begin(baud);
  //Still necessary?!
  return true;
  
  delay(100);
  //Modul needs time to "wakeup"
  if(btVersion == 0)
    delay(BT_Cmd_Delay * 2);
  PrintBtAt();
  BT.flush();
  delay(BT_Cmd_Delay);
  if (BtDataAvailable())
    return true;

  SetError(ERROR_BT_WrongVersionInEeprom);
  btVersion = 0xFF;
  return FindBtModule();
}

bool FindBtModule() {
  delay(BT_Cmd_Delay);
  //First try with CarriageReturn/Linefeed. Otherwise the buffer might be filled with previous value
  btVersion = 3;
  baud = BtFindCurrentBaud();

  //Change Version to use delay() without linebreak
  if (baud == 0L) {
    btVersion = 0;
    baud = BtFindCurrentBaud();
  }

  //Nothing found
  if (baud == 0L) {
    //Reset Version to uninitialized
    btVersion = 0xff;
    baud = BT_BaudFallback;
    //Try  with default value
    BT.begin(baud);    
    SetError(ERROR_BT_BaudNotFound);
    return false;
  }

  //Get Version to execute right method
  if (BtGetVersion())
    SetBtVersion();

  //Clear incoming Buffer
  BtDataAvailable();
  
  //Baud not 57000
  if (baud != BT_Baud)
    if (!ChangeBtBaud()) {
      SetError(ERROR_BT_BaudNotChanged);
      //Stay with found baud or default value
      if (baud == 0L)
        baud = BT_BaudFallback;        
      BT.begin(baud);      
      return false;
    }
  baud = BT_Baud;
  BT.begin(baud);
  return true;
}

long BtFindCurrentBaud() {
  static const long rates[] = { 4800, 9600, 19200, 38400, 57600, 115200 };
  uint8_t numRates = sizeof(rates) / sizeof(long);  //6
  //for (int rn = 0; rn < numRates; rn++) {
  for (int rn = numRates-1; rn >= 0 ; rn--) {
    BT.begin(rates[rn]);    
    BtDataAvailable();
    delay(200);
    PrintBtAt();
    BT.flush();
    if (btVersion >= 3)
      delay(100);
    else
      delay(BT_Cmd_Delay);  //No Linefeed, but 1Sec idle

    if (BtDataAvailable())
      return rates[rn];    
  }
  return 0L;
}

bool BtGetVersion() {
  if(btVersion != 0xFF)
    return BtGetVersion(btVersion >= 3);  
  
  if(BtGetVersion(true))
    return true;
  delay(BT_Cmd_Delay);
  return BtGetVersion(false);
}

bool BtGetVersion(bool linebreak) {
  BT.print(F("AT+VERSION"));
  if (linebreak){
    BT.println();
    delay(200);
  }
  else
    delay(BT_Cmd_Delay);
  char input[40];
  uint8_t j = 0;
  while (BT.available()) {
    if (j < sizeof(input))
      input[j++] = BT.read();
    else
      BT.read();
      
    //0 HC-06-20190901
    //1      
    //2 FC-114 (hc01.comV2)      
    //3 1744  (VERSION:3.0-20170609)
    //4 ZS-40 (Firmware V3.0.6,Bluetooth V4.0 LE)
    //1 ZS-040 (DX_smartv2.0)
    //1 ZS-040 (OKlinvorV1.8)
    //4 BT05 (+VERSION=Firmware V3.0.6,Bluetooth V4.0 LE)
    if (input[0] == 'O' && input[1] == 'K') {
      btVersion = 1;
      return true;
    }
    if (input[0] == 'D' && input[1] == 'X') {
      btVersion = 1;
      return true;
    }
    if (input[8] == 'V' && input[9] == '2') {
      btVersion = 2;
      return true;
    }
    if (input[0] == 'H' && input[1] == 'C') {
      btVersion = 0;
      return true;
    }
    if (input[8] == '3' && input[10] == '0') {
      btVersion = 3;
      return true;
    }
    if (input[10] == '3' && input[27] == '4') {
      btVersion = 4;
      return true;
    }
    if (input[19] == '3' && input[36] == '4') {
      btVersion = 4;
      return true;
    }
  }
  if (j > sizeof(input) - 1)
    j = sizeof(input) - 1;
  input[j] = '\0';
  
  SetError(ERROR_BT_VersionNotFound);
  return false;
}

// AT+BAUD<X> -> OK<X>
// X=4 : 9600bps (Default)
// X=6 : 38400bps
// X=7 : 57600bps
// X=8 : 115200bps
bool ChangeBtBaud() {
  if (btVersion < 3)
    BT.print(F("AT+BAUD7"));
  if (btVersion == 3)
    BT.println(F("AT+UART=57600,0,0"));
  if (btVersion > 3)
    BT.println(F("AT+BAUD7"));
  delay(BT_Cmd_Delay);
  return BtDataAvailable();
}

//  AT : Check the connection.
//  AT+NAME : See default name
//  AT+ADDR : see default address
//  AT+VERSION : See version
//  AT+UART : See baudrate
//  AT+ROLE: See role of bt module(1=master/0=slave)
//  AT+RESET : Reset and exit AT mode
//  AT+ORGL : Restore factory settings
//  AT+PSWD: see default password
bool ProcessBtCommand() {
  if (btBufferCounter < 1)
    return false;

  //Change BT command to AT command:
  if (btBuffer[0] == 'B')
    btBuffer[0] = 'A';
  else
    return false;
    
  SetStatusLed(HIGH);
  WaitForDisconnect();
  SetStatusLed(LOW);

  for (uint8_t i = 0; i < btBufferCounter; i++)
    BtWrite(btBuffer[i]);
  if (btVersion >= 3)
    BtPrintln();

  ClearBtBuffer();
  delay(1000);
  while (BT.available()) {
    char input = BT.read();
    btBuffer[btBufferCounter++] = input;
  }
  
  delay(1000);
  WaitForConnect();

  for (uint8_t i = 0; i < btBufferCounter; i++)
    BT.write(btBuffer[i]);

  return true;
}

void WaitForDisconnect() {
  //BT has to be disconnected to process AT commands
  BT.println(F("Disconnect!"));
  for (uint8_t i = 0; i < 10; i++) {
    PrintBtAt();
    delay(BT_Cmd_Delay);
    char input[3];
    uint8_t j = 0;
    while (BT.available()) {
      if (j < sizeof(input))
        input[j++] = BT.read();
      else
        BT.read();

      //Response OK or AT or +DISC:SUCCESS (BT Modul: 1744 - VERSION:3.0-20170609)
      if ((input[0] == 'O' && input[1] == 'K') || (input[0] == 'A' && input[1] == 'T') || (input[0] == '+' && input[1] == 'D' && input[2] == 'I')) {
        //exit both loops
        i = 100;
        break;
      }
    }
  }
  BtDataAvailable();
}

void WaitForConnect() {
  while (BT.available())
    BT.read();
  for (uint8_t i = 0; i < 10; i++) {
    PrintBtAt();
    delay(BT_Cmd_Delay);
    if (!BtDataAvailable())
      return;
  }
}

void ProcessBtResponse(char input) {
  //+DISC:SUCCESS
  //OK
  if (input != '\0') {
    btBuffer[0] = input;
    btBufferCounter = 1;
  }
  while (BT.available())
    btBuffer[btBufferCounter++] = BT.read();
  ClearBtBuffer();
  //ToDo: is EcuConnected -> Disconnect?
}

void BtEnd(){
  BT.flush();
  BT.end();
}
void BtResume(){
  if(baud != 0)
    BT.begin(baud);
}

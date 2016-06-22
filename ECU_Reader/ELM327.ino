bool ReadBluetoothInput()
{  
  while(BT.available())
  {
    char input = BT.read();      
    //Ignore incoming Spaces:
    if(input == ' ') 
      continue;
           
    elmRequest[counter++] = input;
    
    delay(BTDelay);
    
    //Carriage Return
    if(input == 0x0D)
      break;    
  }

  //Nothing to read:
  if(counter == 0)
    return false;

  //BT Connected sucessfully
  bluetoothConnected = true;
  lastBTrequest = millis();
      
  //Repeat last command when only \r appears
  if(counter == 1 && lastPID != 0x00)
  {
    ClearBuffer();
    return sendPID(translatedPID);    
  }

  //Interpret AT Command (Controls ELM327)
  if(elmRequest[0] == 'A' && CheckAT())
    return true;
      
  //Respond PID-List (Available Parameter)
  //if(counter >= 4)
  if(elmRequest[0] == '0' && elmRequest[1] == '1' && elmRequest[3] == '0' && ReceivePIDs())
    return true; 
        
  //PID Request (Mode01) (Show current Data)
  //if(counter >= 3)  
  if(elmRequest[0] == '0' && elmRequest[1] == '1' && CheckPID())
    return true;

  //PID Request (ModeXX) (Freezed data, Error codes, ect.)
  //if(counter >= 4)  
  if(elmRequest[0] == '0' && CheckPIDMode())
      return true;
        
  //Linefeed -> Ignore (Sometimes sent after CarriageReturn)
  if(elmRequest[counter] == 0x0A)
  {
    ClearBuffer();
    return true;
  }
  
  //ToDo:
  //If last Request older then 20 Seconds: Answer with "?"
  //Unknown Command:
  return false;
}

bool CheckPID()
{  
  //Remove Header, if required
  //otherwise convert Text to Hex ("0A" -> 0x0A)
  lastPID = translateIncomingHeader();

  //Change ID´s to KDS pendant:
  TranslateServiceId();

  //Send only if response not faked
  if(TranslateParameterId())  
    return sendPID(translatedPID);
}

//Mode
void TranslateServiceId()
{
    if(elmRequest[0] != '0')
      return;
    lastSID = GetByteFromHexString(String(elmRequest[0]) + String(elmRequest[1]));
       
    switch(elmRequest[1])
    {
      case '1': //Show current data
        translatedSID = 0x21;
      break;
      case '2': //Show freeze frame data
        translatedSID = 0x12;
      break;
      case '3': //Show stored Diagnostic Trouble Codes
        translatedSID = 0x13;
      break;
      case '4': //Clear Diagnostic Trouble Codes and stored values
        translatedSID = 0x14;
      break;
      case '5': //Test results, oxygen sensor monitoring (non CAN only)        
        translatedSID = lastSID;
      break;
      case '6': //Test results, other component/system monitoring (Test results, oxygen sensor monitoring for CAN only)        
        translatedSID = lastSID;
      break;
      case '7': //Show pending Diagnostic Trouble Codes (detected during current or last driving cycle)
        translatedSID = 0x17;
      break;
      case '8': //Control operation of on-board component/system      
        translatedSID = lastSID;
      break;
      case '9': //Request vehicle information
//        0x02: Vehicle Identification Number
//        0x04: Calibration Identifications
//        0x06: Calibration Verification Number
//        0x08: In-use Performance Tracking (Spark)
//        0x0A: ECU Name
//        0x0B: In-use Performance Tracking (Compression)
//        0x0D: Engine Serial Number
//        0x0F: Exhaust Regulation Or Type Approval Number 
        translatedSID = 0x1A; //???? 1A is ECU Id...
      break;
      case 'A': //Permanent Diagnostic Trouble Codes (DTCs) (Cleared DTCs)        
        translatedSID = lastSID;
      default:
        translatedSID = lastSID;
      break;
    }
}

//Translate ISO (OBD II) to Kawasaki Request
bool TranslateParameterId()
{  
  switch(lastPID)
  {    
    case 0x04: //Engine Load 1 Byte - OK
      //Throttle Opening & Engine Speed (RPM)
      //Or Inlet Air vacuum & RPM
      //ToDo: Use for Gear and calc within Virb Edit
      translatedPID = 0x5B;   //???
      //translatedPID = 0x0B;
    break;
    case 0x05: //Engine Coolant Temperature 1 Byte - OK
      translatedPID = 0x06;
    break;
    case 0x0B: //Intake Manifold Air Pressure 1 Byte
      translatedPID = 0x05; //Inlet Air Pressure
    break;
    case 0x0C: //Engine RPM 2Byte - OK
      translatedPID = 0x09;
    break;
    case 0x0D: //Speed 1 Byte - OK
      translatedPID = 0x0C;
    break;
    case 0x0F: //Intake Air Temperature 1 Byte - OK
      translatedPID = 0x07;
    break;
    case 0x10: //MAF 2 Byte - OK [Not supported by garmin]
      translatedPID = 0x05;  //MAF is nearly the same like Intake Air Pressure
    break;
    case 0x11: //Throttle Position 1 Byte - OK
      translatedPID = 0x04;
    break;    
    case 0x1E:  //Wildcard for Gear
      translatedPID = 0x0B;
    break;
    case 0x33: //Barometric Pressure 1 Byte
      translatedPID = 0x08; //Atmospheric Pressure
    break;    
    case 0x42: //Internal Control Voltage 2 Byte
      //Voltage Internal C2
      //translatedPID = 0x5F;
      //Battery Voltage
      translatedPID = 0x0A;
    break;
    case 0x45: //Relative Throttle Position 1 Byte
      translatedPID = 0x5B;
    break;    
    case 0x1C: //OBD standard this vehicle conforms to 1 Byte
    case 0x4F: //Maximum values Fuel, Oxy, Voltage 4 Byte
    case 0x50: //Maximum values Air flow rate 4 Byte 
    case 0x51: //Fuel Type 1 Byte
      //Fake Return Values
      //return fakePID(lastPID);
      if(fakePID(lastPID))      
        return false;
    break;
    case 0x7F:  //TotalOperatingHours    
      translatedPID = 0x44;
    break;    
    default:
      translatedPID = 0x00;
    break;
  }
  return true;
}

bool CheckPIDMode()
{  
  //Remove Header, if required
  //otherwise convert Text to Hex ("0A" -> 0x0A)
  lastPID = translateIncomingHeader();

  TranslateServiceId();
  //ToDo: SendPID, Fake Values or Send different Commands..  
  ClearBuffer();
  //ToDo: Kills Emulator at the moment ;-)
  return true;
  return sendPID(lastPID);
}

bool sendPID(uint8_t pid)
{  
  if(!ECUconnected)
  {
    //Try to open protocol
    BT.println("SEARCHING...");
    fastInit();
  }

  if(ECUconnected && pid != 0x00)
  { 
    //Send PID to ECU
    if(processRequest(pid))
    {
      //Error Respondend from ECU! Re-Init after 2 Seconds
      if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
      {        
        //Error responded
        BT.print("?");
        ErrorAppeard();
        ClearBuffer();
        return false;
      }
    }    
  }
  else //Not connected
    if(pid != 0x00)
    {
      BT.print("BUSY");
      ClearBuffer();
      return false;
    }

  //Unknown PID or empty Response:
  if(pid == 0x00 || (ecuResponse[2] == 0x00 && ecuResponse[3] == 0x00))
  {    
    BT.print("NO DATA");
  }
  else
  {    
    uint8_t elmResponse[12];
    int responseCounter = 0;

    //Calculate Values fom KDS to OBD II
    ConvertResult();    
    
    //header required:
    if(header)
    {
      //80 F1 11 03 61 0B 01 F2
      //80 - Format/Adress
      //   F1 - Sender/Absender
      //      11 - Target/Empfänger
      //         03 - Lenght
      //            61 - ServiceID | ELM327 41 (!!!)
      //               0B - Mode/PID/Befehl
      //                  01 - Value1
      //                  [XX - Value2]
      //                     F2 - Checksum
      
      elmResponse[responseCounter++] = 0x80;                
      elmResponse[responseCounter++] = MyAddr;
      elmResponse[responseCounter++] = ECUaddr;
      //ToDo: Make dynamically
      if(ecuResponse[3] == 0x00)
        elmResponse[responseCounter++] = 0x03;//2 byte response + ServiceID + lastPid
      else
        elmResponse[responseCounter++] = 0x04;//3 byte response
    }
    //ServiceID (OBD2: 61, KDS: 41) [Always: SID + 40]    
    elmResponse[responseCounter++] = lastSID + 0x40;
    
    //requested PID (not translated PID!!!)
    elmResponse[responseCounter++] = lastPID;
    
    //1st Byte
    elmResponse[responseCounter++] = ecuResponse[2];
    
    //2nd Byte
    if(ecuResponse[3] != 0x00)    
      elmResponse[responseCounter++] = ecuResponse[3];

    //ToDo: 6Bytes on Total Operating Hours!!!
    //Dirty hack oO
    if(pid == 0x44)
    {
      //Length
      elmResponse[3] = 0x06;
      elmResponse[responseCounter-1] = 0x00;
      elmResponse[responseCounter] = 0x00;
      elmResponse[responseCounter++] = ecuResponse[2];
      elmResponse[responseCounter++] = ecuResponse[3];      
      elmResponse[responseCounter++] = ecuResponse[4];
      elmResponse[responseCounter++] = ecuResponse[5];      
    }
    
    //Calc Checksum if required
    if(header)    
      elmResponse[responseCounter] = calcChecksum(elmResponse, responseCounter);
    else
      responseCounter--;
    for(int i = 0; i <= responseCounter; i++)
    {      
      PrintHex(elmResponse[i]);
      if(i < responseCounter)
        SendSpace();
    }    
  }  
  SendNewLine();
  SendPrompt();  
  ClearBuffer();
  return true;
}

bool fakePID(uint8_t pid)
{
  //Empty previous "results"
  ClearBuffer();
  int byteCount = 0;
  switch(pid)
  {
    case 0x1C: //OBD compliant
      byteCount = 1;
      ecuResponse[0] = 0x05; //Non OBD compliant
      break;
    case 0x4F: //Maximum values Fuel, Oxy, Voltage 4 Byte
      byteCount = 4;
      ecuResponse[0] = 0x00;
      ecuResponse[1] = 0x00;
      ecuResponse[2] = 0x00;
      ecuResponse[3] = 0x00;      
    break;
    case 0x50: //Maximum value for AirFlow Rate (4Byte) 0-25550
      byteCount = 4;
      ecuResponse[0] = 0x00;
      ecuResponse[1] = 0x00;
      ecuResponse[2] = 0x00;
      ecuResponse[3] = 0x00;      
    break;
    case 0x51: //Fuel Type
      byteCount = 1;
      ecuResponse[0] = 0x01; //Gasoline
    break;
    default:
//      BT.print("NO DATA");
//      SendNewLine();
//      SendPrompt();  
//      ClearBuffer();
      return false;
  }
  
  uint8_t elmResponse[12];
  int responseCounter = 0; 
  //header required:
  if(header)
  {    
    elmResponse[responseCounter++] = 0x80;                
    elmResponse[responseCounter++] = MyAddr;
    elmResponse[responseCounter++] = ECUaddr;
    //Length
    elmResponse[responseCounter++] = byteCount + 2; //Data + ServiceID + PID    
  }
  //ServiceID (OBD2: 61, KDS: 41)  
  elmResponse[responseCounter++] = lastSID + 0x40;
  
  //requested PID (not translated!!!)
  elmResponse[responseCounter++] = lastPID;
  
  //Return values
  for(int i = 0; i < byteCount; i++)
    elmResponse[responseCounter++] = ecuResponse[i];
   
  //Calc Checksum if required
  if(header)    
    elmResponse[responseCounter] = calcChecksum(elmResponse, responseCounter);
  
  for(int i = 0; i <= responseCounter; i++)
  {      
    PrintHex(elmResponse[i]);
    if(i < responseCounter)
      SendSpace();
  }
  
  SendNewLine();
  SendPrompt();  
  ClearBuffer();
  return true;
}

void ConvertResult()
{
  unsigned int value = 0;
  uint8_t minimum;
  float decValue;
  switch(ecuResponse[1])
  {
    case 0x04: //Throttle Position Sensor
      //201 = 0% = idle, 405 = 100%
      //Sometimes 202 (C9 | CA) 
      minimum = 201;
      
      value = ecuResponse[2] * 100 + ecuResponse[3];
            
      if(value > ThrottlePosMax)
      {
        ThrottlePosMax = value;
        EEPROM.write(0, ThrottlePosMax - 255);        
      }      
      //((Value-Minimum) *100) / (Maximum - Minimum) = %      
      //OBD II Calculation: (100/255) * A [Backwards]: 100% = 255
      if(value > minimum)        
        ecuResponse[2] = ((value-minimum) *100) / (ThrottlePosMax - minimum) * 255/100;
      else
        value = 0x00;
            
      ecuResponse[3] = 0x00;
      break;
    case 0x05: //Airpressure: From 2 byte to 1 byte:    
      if(ecuResponse[2] >= 2)
        ecuResponse[2] = ecuResponse[2] / 2; //Double precision
      //Ignore accuracy
      ecuResponse[3] = 0x00;      
      break;    
    case 0x06: //Temp
    case 0x07:
      //(A-48)/1.6 = Celsius
      //+40 to avoid negative values
      if(ecuResponse[2] == 0x00)
        ecuResponse[2] = 40; //0C°
      else
        ecuResponse[2] = (ecuResponse[2] -48) / 1.6 + 40;
    break;
    case 0x08:
      //Pressure in kPa
      //0 Meters above Sea level = 100 kPa
      if(ecuResponse[2] >= 2)
        ecuResponse[2] = ecuResponse[2] /2;
    break;
    case 0x09: //RPM
      value = ecuResponse[2] *100 + ecuResponse[3];      
      //OBD: (256A + B) / 4
      value *= 4;
      ecuResponse[3] = value % 256;      
      if(value >= 256)
        ecuResponse[2] = value / 256;
      else
        ecuResponse[2] = 0x00;
    break;
    case 0x0B: //Gear    
      //Delete KDS Checksum
      ecuResponse[3] = 0x00;      
    break;
    case 0x0C: //Speed    
      //(A*100+B) / 2
      value = ecuResponse[2] *100 + ecuResponse[3];      
      if(value >= 2)
        ecuResponse[2] = value / 2;
      else
        ecuResponse[2] = 0x00;
      ecuResponse[3] = 0x00;
    break;    
    case 0x5B: //Sub Throttle valve Sensor
      //81 = 0% = idle, (?)189 = 100% //will be adjusted dynamically
      minimum = 81;
      value = ecuResponse[2];      
      if(value < minimum)
        value = minimum;
      if(value > SubThrottleMax)
      {
        SubThrottleMax = value;
        EEPROM.write(1, SubThrottleMax);
      }
      //((Value-Minimum) *100) / (Maximum - Minimum) = %      
      //OBD: A*100/255      
      ecuResponse[2] = ((value-minimum) *100) / (SubThrottleMax - minimum) * 255/100;
    break;
    case 0x5F: //Internal Voltage
    case 0x0A:  //Battery Voltage    
      value = ecuResponse[2];
    //153 = 12 V = Factor 12,75
    //OBD II
    //((A*256)+B)/1000
    //2 Byte
    decValue = ecuResponse[2] / 12.75;
    ecuResponse[2] = ecuResponse[2] / 12.75 * 1000/256;
    ecuResponse[3] =  100 * (decValue - value);
    break;
    default:
    break;
  }
}

bool ReceivePIDs()
{     
  SendEcho();
  uint8_t elmResponse[12];
  int responseCounter = 0;
  
  if(header)
  {    
      elmResponse[responseCounter++] = 0x80;                
      elmResponse[responseCounter++] = MyAddr;
      elmResponse[responseCounter++] = ECUaddr;      
      elmResponse[responseCounter++] = 0x06; //Length
  }
  //ServiceID  
  elmResponse[responseCounter++] = lastSID + 0x40;
  
  //Last Request
  elmResponse[responseCounter++] = GetByteFromHexString(String(elmRequest[2]) + String(elmRequest[3]));
  
  switch(elmRequest[2])
  {
    case '0':            
      //Engine Coolant Temp | Intake Pressure | RPM | Speed | Intake Air Temp | Throttle | PIDs 33-64
      //ALT
      //00011000000110111000000000000001
      //181F8001     
      //NEU mit gear
      //00011000001110111000000000000101
      //Ignore EngineLoad, AirFlowRate
      elmResponse[responseCounter++] = 0x18;
      elmResponse[responseCounter++] = 0x3B;
      elmResponse[responseCounter++] = 0x80;
      elmResponse[responseCounter++] = 0x05;
    break;
    case '2':
      //412000000001
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x20;
      elmResponse[responseCounter++] = 0x01;
    break;
    case '4':
      //414008000000
      //Relative throttle position in %
      elmResponse[responseCounter++] = 0x48;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x01;
    break;
    case '6':            
      //416000000000
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x02;
    break;
    case '8':
      //418000000001
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x01;
    break;
    case 'A':
      //41A000000001
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x01;
    break;
    case 'C':
      //41C000000000
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
    break;
    default:
    return false;
  }
  //Calc Checksum
  if(header)   
    elmResponse[responseCounter] = calcChecksum(elmResponse, responseCounter);
  else
    responseCounter--;
  for(int i = 0; i <= responseCounter; i++)
  {      
    PrintHex(elmResponse[i]);
    if(i < responseCounter)
      SendSpace();
  }
  SendNewLine();
  SendPrompt();  
  ClearBuffer();    
  return true;            
}

bool CheckAT()
{
  bool ok = false;  
  switch(elmRequest[2])
  {
    case 'A': //Adaptive Timing
      if(elmRequest[3] == 'T')
      {        
        switch(elmRequest[4])
        {
          case 0: //Disable
          case 1: //Set by ST
          case 2: //More agressive...
          ok = true;  
          break;
        }        
      }
      break;      
    case 'D': //Default
      //Describe protocol by number:
      if(elmRequest[3] == 'P' && elmRequest[4] == 'N')
      {
        //ISO 14230-4 KWP (fast init, 10.4 kbaud)
        //BT.print(5);
        //Auto
        BT.print(0);
        ok = false;
      }//Describe protocol:
      else if(elmRequest[3] == 'P')
      {
        //ISO 14230-4 KWP (fast init, 10.4 kbaud)
        //BT.print('ISO 14230-4 KWP');
        //If Protocol is set to Auto:
        BT.print('AUTO, ISO 14230-4 KWP');
        ok = false;
      }
      else //Reset settings to Default
      {
        header = false;
        spaces = false;
        linefeed = false;
        echo = false;
        memory = false;
        ok = true;
      }
    break;
    case 'Z': //Reset
      if(ECUconnected)
        stopComm();
      if(!ECUconnected)
        ECUconnected = fastInit();
      BT.print(elmVersion);
      SendNewLine();
      ok = true;
    break;         
    case 'H': //Header            
      header = elmRequest[3] == '1';
      ok = true;
    break;
    case 'I': //Device Info      
      BT.print(elmVersion);
      ok = false;
    break;
    case 'S': //Protocol Auto (SP) | Space (S0 / S1)
      if(elmRequest[3] == 'P')//Protocol Auto
        ok = true;
      else//Space
      {              
        spaces = elmRequest[3] == '1';
        ok = true;
      }            
    break;
    case 'L': //lineFeed            
      linefeed = elmRequest[3] == '1';
      ok = true;
    break;
    case 'E': //Echo            
      echo = elmRequest[3] == '1';
      ok = true;
    break;
    case 'M': //Memory      
      memory = elmRequest[3] == '1';            
      ok = true;
    break;
    case 'P': //ProtocolClose
      if(elmRequest[3] == 'C')
      {
        stopComm();
        ok = true;
      }
      break;
    case 'W': //WarmStart
      if(elmRequest[3] == 'S')
      {
        //ToDo: Restart?
        BT.print(elmVersion);
        ok = false;
      }
    break;
    case ' ':
      if(elmRequest[3] == 'A' && elmRequest[4] == 'T')
      {
        //Adaptive Timing:
        //0 = Off
        //1 = Auto1
        //2 = Auto2              
        ok = true;
      }
      if(elmRequest[3] == 'H')
      {              
        header = elmRequest[4] == '1';             
        ok = true;
      }
    break;
    case '@':
      switch(elmRequest[3])
      {
        case '1': //Device Description
          BT.print("Thomas KDS Reader");
          ok = false;
        break;
        case '2': //Device Identifier
          BT.print("ELM327");
          ok = false;
        break;
        case '3': //Set Device Identifiier (Nope :P )          
          ok = true;
        break;
      }      
    break;
    case 'X': //My sniffer features
      ok = CustomAtCommands(elmRequest[3]);
    break;
    default: //Unknown            
      BT.print("?");
      ok = false;
    break;
  }
    
  SendEcho();
  
  ClearBuffer();
  if(ok)
    SendOK();
  SendNewLine();
  SendPrompt();
  return true;      
}

bool CustomAtCommands(char c)
{
  switch(c)
  {
    case 'A': //Receive all supported PIDs and Values
      SniffEcu();      
      ClearBuffer();
      return true;
    break;
    case 'S': //Receive some specific PID Values
      SniffKnownCmds();
      ClearBuffer();
      return true;
    break;
    case 'C': //Clear Throttle max values and reset to stock
      ClearThrottleValues();
      SendOK();
      return true;
    break;
    case 'P': //Print current Throttle Values
      PrintThrottleValues();
      return true;
    break;
    case 'D': //Read diagnostic values
      SniffDiagnostic();
      return true;
    break;
    case 'I': //Sniff ECU ID
      SniffInfo();
      return true;
    break;
    default:
      BT.print("?");
      return false;    
  }
}

void ClearThrottleValues()
{
  //Lower for self adjustment
  //ThrottlePosMax = 405;
  ThrottlePosMax = 380;
  EEPROM.write(0, ThrottlePosMax -255);
  //SubThrottleMax = 189;
  SubThrottleMax = 170;
  EEPROM.write(1, SubThrottleMax);
}

void PrintThrottleValues()
{
  BT.print("Throttle: ");
  BT.println(ThrottlePosMax);
  BT.print("SubThrottleValve: ");
  BT.println(SubThrottleMax);
}

uint8_t translateIncomingHeader()
{
  String value;
  //With Header & Spaces
  //80 11 F1 02 21 06 AB
  //80 - Format/Adress
  //   11 - Target/Empfänger
  //      F1 - Sender/Absender
  //         02 - Lenght
  //            21 - Mode/ServiceId/Befehl
  //               06 - Parameter/Register
  //                  AB - Checksum
  if(header && counter >= 9)
  {
    if(spaces)
      value = String(elmRequest[8 + 4]) + String(elmRequest[9 + 4]);
    else
      value = String(elmRequest[8]) + String(elmRequest[9]);
  }
  else
    value = String(elmRequest[2]) + String(elmRequest[3]);
  value.toUpperCase();    
  return GetByteFromHexString(value);
}

void SendNewLine()
{ 
  //Carriage Return 13
  BT.write(0x0D);
  //LineFeed 10
  if(linefeed)
    BT.write(0x0A);     
}

void SendPrompt()
{   
   //Prompt: >
   BT.write(0x3E);
}

void SendSpace()
{
  if(spaces) 
    BT.write(0x20);
}

void SendOK()
{
  BT.print("OK");  
}

void SendEcho()
{
  if(echo)  
  {      
    for(int i = 0; i <= counter; i++)
    {
      if(elmRequest[i] == 0x00)
        break;
      BT.write(elmRequest[i]);
    }        
    SendNewLine();
  }
}

void ClearBuffer()
{
  counter = 0;  
  memset(elmRequest, 0, sizeof(elmRequest));
}

void PrintHex(uint8_t data) // prints 8-bit data in hex with leading zeroes
{
  if (data < 0x10)         
    BT.print("0");
  String str = String(data,HEX);
  str.toUpperCase();
  BT.print(str);
}

byte GetByteFromHexString(String hexValue)
{
  return getVal(hexValue[1]) + (getVal(hexValue[0]) << 4);
}

byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else
     return (byte)(c-'A'+10);
}

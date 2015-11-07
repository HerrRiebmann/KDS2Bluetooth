bool ReadBluetoothInput()
{  
  while(BT.available())
  {
    char input = BT.read();      
    //Ignore incoming Spaces:
    if(input == ' ') 
      return false;
    
    bluetoothConnected = true;
    lastBTrequest = millis();
    
    elmRequest[counter++] = input;
    delay(3);
    //Carriage Return
    if(input == 0x0D)
      break;    
  }

  //Nothing to read:
  if(counter == 0)
    return false;
    
  //Repeat last command when only \r appears
  if(counter == 1 && lastPID != 0x00)
  {
    return sendPID(translatedPID);    
  }

  //Interpret AT Command
  if(elmRequest[0] == 'A')
    if(CheckAT())
      return true;
      
  //Respond PID-List
  if(counter >= 4)
    if(elmRequest[0] == '0' && elmRequest[1] == '1' && elmRequest[3] == '0')       
      if(ReceivePIDs())
        return true; 
        
  //PID Request (Mode01)
  if(counter >= 3)  
    if(elmRequest[0] == '0' && elmRequest[1] == '1')  
      if(CheckPID())
        return true;
    
  //Linefeed -> Ignore (Sometimes sent after CarriageReturn)
  if(elmRequest[counter] == 0x0A)
    ClearBuffer();
  
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

  //Abs Pressure (Kein Standard) - 08 
  //Gear (Kein Standard) - 0B  
  //Atmospheric Pressure 08
  //Battery Voltage 0A
  //Fuel Cut Mode (Schiebebetrieb)
  
  
  //Translate ISO (OBD II) to Kawasaki Request
  switch(lastPID)
  {    
    case 0x04: //Engine Load 1 Byte - OK
      //Throttle Opening & Engine Speed (RPM)
      //Or Inlet Air vacuum & RPM
      translatedPID = 0x5B;   //???
    break;
    case 0x05: //Engine Coolant Temperature 1 Byte - OK
      translatedPID = 0x06;
    break;
    case 0x10: //MAF 2 Byte - OK [Nicht garmin] 
      translatedPID = 0x05;  //MAF is nearly the same like Intake Air Pressure
    break;
    case 0x11: //Throttle Position 1 Byte - OK
      translatedPID = 0x04;
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
    case 0x33: //Barometric Pressure 1 Byte
      translatedPID = 0x08; //Atmospheric Pressure
    break;    
    case 0x42: //Internal Control Voltage 2 Byte
      translatedPID = 0x5F;
    break;
    case 0x45: //Relative Throttle Position 1 Byte
      translatedPID = 0x5B;      //???
    break;    
    default:
      translatedPID = 0x00;
    break;
  }
  return sendPID(translatedPID);  
}

bool sendPID(uint8_t pid)
{  
  if(!ECUconnected)
    ECUconnected = fastInit();

  if(ECUconnected && pid != 0x00)
  { 
    if(processRequest(pid))
    {
      //Error Respondend from ECU! Re-Init after 2 Seconds
      if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
      {        
        //Error responded
        BT.print("?");
        ErrorAppeard();            
        return false;
      }
    }    
  }
  else //Not connected
    if(pid != 0x00)
    {
      BT.print("BUSY");      
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
      if(ecuResponse[3] == 0x00)
        elmResponse[responseCounter++] = 3;
      else
        elmResponse[responseCounter++] = 4;
    }
    //ServiceID
    elmResponse[responseCounter++] = 0x41;    
    
    //requested PID (not translated!!!)
    elmResponse[responseCounter++] = lastPID;

    //Calculate Values fom KDS to OBD II
    ConvertResult();    
    
    //1st Byte
    elmResponse[responseCounter++] = ecuResponse[2];
    
    //2nd Byte
    if(ecuResponse[3] != 0x00)    
      elmResponse[responseCounter++] = ecuResponse[3];
    
    //Calc Checksum if required
    if(header)
      elmResponse[responseCounter] = calcChecksum(elmResponse, responseCounter);
    for(int i = 0; i < responseCounter; i++)
    {      
      PrintHex(elmResponse[i]);
      SendSpace();
    }    
  }  
  SendNewLine();
  SendPrompt();  
  ClearBuffer();
  return true;
}

void ConvertResult()
{
  int value = 0;
  switch(ecuResponse[1])
  {
    case 0x04: //Throttle Position Sensor
      //201 = 0% = idle, 405 = 100%
      //((Value-Minimum) *100) / (Maximum - Minimum)
      value = ecuResponse[2];
      if(value >= 201)
        value = ((value-201) *100) / (405 - 201);
      ecuResponse[2] = value;
      ecuResponse[3] = 0x00;
      break;
    case 0x05: //Airpressure: From 2 byte to 1 byte:      
      ecuResponse[2] = ecuResponse[2] / 2;    
      //Ignore accuracy
      ecuResponse[3] = 0x00;      
      break;
    case 0x08:
      //Pressure in kPa
      ecuResponse[2] = ecuResponse[2] /2;
    break;
    case 0x06: //Temp
    case 0x07:      
      value = ecuResponse[2] -48;
      value /= 1.6;      
      value += 40;
      ecuResponse[2] = value;
    break;
    case 0x09: //RPM
      value = ecuResponse[2] *100;
      value += ecuResponse[3];
      value *= 4;
      ecuResponse[3] = value % 100;
      ecuResponse[2] = (value - ecuResponse[3]) / 256;
    break;
    case 0x0C: //Speed    
      //(A*100+B) / 2
      value = ecuResponse[2] *100;
      value += ecuResponse[3];    
      if(value > 0)
        value /= 2;    
      ecuResponse[2] = value;   
      ecuResponse[3] = 0x00;
    break;
    case 0x5B: //Throttle Pos. Sensor
      //81 = 0% = idle, (?)189 = 100% //Not yet sure if max Value
      //((Value-Minimum) *100) / (Maximum - Minimum)
      value = ecuResponse[2];
      if(value >= 81)
        value = ((value-81) *100) / (189 - 81);
      ecuResponse[2] = value;
    break;
    case  0x5F: //Voltage
    //153 = 12 V = Factor 12,75
      value = ecuResponse[2] / 12.75;
    break;
  }
}

bool ReceivePIDs()
{     
  SendEcho();      
  switch(elmRequest[2])
  {
    case '0':            
      //Engine Coolant Temp | Intake Pressure | RPM | Speed | Intake Air Temp | Throttle | PIDs 33-64
      //00011000000110111000000000000001
      //181B8001
      if(spaces)
        BT.write("41 00 18 1B 80 01");
      else
        BT.write("4100181B8001");
    break;
    case '2':
      //PIDs 65-96
      if(spaces)
        BT.write("41 20 00 00 00 01");
      else
        BT.write("412000000001");
    break;
    case '4':
      //Relative throttle position in %
      if(spaces)
        BT.write("41 40 08 00 00 00");
      else
        BT.write("414008000000");                         
    break;
    case '6':            
      if(spaces)
        BT.write("41 60 00 00 00 00");
      else
        BT.write("416000000000");            
    break;
    case '8':            
      if(spaces)
        BT.write("41 80 00 00 00 00");
      else
        BT.write("418000000000");
    break;
    default:
    return false;
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
    case 'D': //Default            
      header = false;
      spaces = false;
      linefeed = false;
      echo = false;
      memory = false;
      ok = true;
    break;
    case 'Z': //Reset            
      if(!ECUconnected)
        fastInit();
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
    case 'X': //Receive all supported PIDs and Values
      SniffEcu();      
      ClearBuffer();
      return true;
    break;
    case 'Y': //Receive some specific PID Values
      SniffKnownCmds();
      ClearBuffer();
      return true;
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
        return;
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

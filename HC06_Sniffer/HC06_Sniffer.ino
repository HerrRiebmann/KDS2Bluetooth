#include <SoftwareSerial.h>
SoftwareSerial BT(11, 10); 
// creates a "virtual" serial port/UART
// connect BT module TX to D10
// connect BT module RX to D11
// connect BT Vcc to 5V, GND to GND

//Send Header Info (Sender, Receiver, ect.)
bool headerInfo = false;
//Disable Spaces between Values
bool spaces = true;
//Disable lineFeed
bool lineFeed = true;
//EnableEcho
bool echo = false;
bool memory = false;

bool EcuConnected = false;

char elmVersion[11] = {'E', 'L', 'M', '3', '2', '7', ' ','v','1','.','4'};
//Incoming Data (Commands)
char cmds[6];
int counter = 0;

void setup()  
{
  Serial.begin(57600);  
  // set digital pin to control as an output
  pinMode(13, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Rename BT Module:
  //BT.print("AT+NAMEThomasZ750r");
  Serial.println("Ready");
}

void loop()
{
  if(BT.available())
  {    
    ReadInput();    
  }
}

bool ReadInput()
{  
  while(BT.available())
  {
    char input = BT.read();      
    //Ignore incoming Spaces:
    if(input == ' ') 
      continue;
        
    cmds[counter++] = input;
    delay(3);
    //Carriage Return
    if(input == 0x0D)
      break;    
  }
      
  if(counter == 0)
    return false;

  
  //Repeat last command when only \r appears
  if(counter == 1 && cmds[counter] == 0x0D)
  {
    Serial.println("Repeat last Command");
    ClearBuffer();
    return true;
  }

  if(cmds[0] == 'A')
    if(CheckAT())
      return true;
  if(cmds[0] == 'S')
    if(CheckST())
      return true;
  if(!EcuConnected)
  {
    Serial.println("Connect ECU");
    delay(50);
    BT.println("SEARCHING...");
    EcuConnected = true;
  }
  if(cmds[0] == '0' && cmds[1] == '1' && cmds[3] == '0')
    if(ReceivePIDs())
      return true; 
  if(cmds[0] == '0'  && cmds[1] == '1')
    if(CheckPID())     
      return true;
  if(counter >= 4)  
  if(cmds[0] == '0')  
    if(CheckService())
      return true;

  //Linefeed -> Ignore (Sometimes sent after CarriageReturn)
  if(cmds[counter] == 0x0A)
  {
    Serial.println("Linefeed...");
    ClearBuffer();
    return true;
  }
    //Unknown Command:
    Serial.println("Unknown: ");
    for(int i = 0; i <= counter; i++)      
      Serial.print(cmds[i]);
    Serial.println();
    ClearBuffer();  
  
  //ToDo:
  //If last Request older then 20 Seconds: Answer with "?"
  
  return false;
}

int f = 0;
bool CheckPID()
{
  if(counter >= 3)
  //Mode01:
  if(cmds[0] == '0' && cmds[1] == '1')
  {
    //Translated Kawasaki Request
    uint8_t request;
    String response;
    String value = String(cmds[2]) + String(cmds[3]);
    value.toUpperCase();    
    uint8_t pid = GetByteFromHexString(value);    
    switch(pid)
    {
      //Abs Pressure (Kein Standard) - 08 
      //Gear (Kein Standard) - 0B

      case 0x04: //Engine Load 1 Byte - OK
        request = 0x00;
        response = "52"; //82%
        Serial.println("Engine Load");
      break;
      case 0x05: //Engine Coolant Temperature 1 Byte - OK
        request = 0x06;
        response = "92"; //106°
        Serial.println("Engine Cool Temp");
      break;
      case 0x10: //MAF 2 Byte - OK [Nicht garmin]
        request = 0x00;
        response = "D9C8"; //557,52 gram/sec
        Serial.println("MAF");
      break;
      case 0x11: //Throttle Position 1 Byte - OK
        request = 0x04;
        response = "DE"; //87%
        Serial.println("Throttle");
      break;
      case 0x0B: //Intake Air Pressure 1 Byte
        request = 0x05;
        response = "7B"; //123kPa
        Serial.println("Intake Air Pressure");
      break;
      case 0x0C: //Engine RPM 2Byte - OK
        request = 0x09;      
        if(f == 0)
          response = "0B23"; //11035
        if(f == 1)
          response = "0DCA"; //13202
        if(f == 2)
          response = "0ACB"; //10203
        if(f >= 3)
          response = "0A23"; //10035
          Serial.println("RPM");
      break;
      case 0x0D: //Speed 1 Byte - OK
        request = 0x0C;
        if(f == 0)
          response = "D8"; //216
        if(f == 1)
          response = "D6"; //214
        if(f == 2)
          response = "D1"; //209
        if(f >= 3)
          response = "D4"; //212
          Serial.println("Speed");
      break;
      case 0x0F: //Intake Air Temperature 1 Byte - OK
        request = 0x07;
        response = "3E"; //22°
        Serial.println("Intake Air Temp");
      break;
      case 0x45: //Relative Throttle Position 1 Byte
        request = 0x00;
        response = "48"; //72%
        Serial.println("Relative Throttle Pos");
      break;
//Neu      
      case 0x4F: //Maximum values 4 Byte
        request = 0x00;
        response = "00000000"; //
        Serial.println("Maximum values Fuel, Oxy, Voltage");
      break;
      case 0x50: //Maximum values Air flow rate 4 Byte
        request = 0x00;
        response = "00000000"; //
        Serial.println("Maximum values Air Flow");
      break;
//Neu      
      default:
        Serial.println("Unknown PID");
        request = 0x00;
      break;
    }

    if(response == "")
    {
      Serial.print(pid);
      Serial.println(" No Data");
      BT.write("NO DATA");    
    }
    else
    {
      //Valid + Sender + ECU-Adress + Length + PID
      if(headerInfo)
        BT.write("80 F1 11 03");
      BT.write("41");
      SendSpace();
      BT.write(cmds[2]);
      BT.write(cmds[3]);  
      
      if(spaces)
      {
        for(int j = 0; j < response.length(); j+=2)
        {
          SendSpace();
          BT.write(response[j]);
          BT.write(response[j+1]);          
        }
      }
      else
        BT.print(response);
      //ToDo: Calculate
      //Checksum
      if(headerInfo)
        BT.write("03");
    }

    SendNewLine();
    SendPrompt();
    //SendNewLine();
    ClearBuffer();     
    return true;    
  }
  return false;
}

bool ReceivePIDs2()
{
    if(counter >= 4)
      if(cmds[0] == '0' && cmds[1] == '1' && cmds[3] == '0')      
      { 
        SendEcho();
        if(headerInfo)
          BT.write("80 F1 11 06");
        switch(cmds[2])
        {
          case '0':
            //Serial.println("PIDs 00-19");
            //Engine Coolant Temp | Intake Pressure | RPM | Speed | Intake Air Temp | Throttle
            //00001000001110101000
            //BT.write("41000083A8000"); 
            
            //Spec says 32 PIDs not 20...
            //Engine Coolant Temp | Intake Pressure | RPM | Speed | Intake Air Temp | Throttle
            //00001000001110101000000000000000
            //083A8000
            Serial.println("PIDs 01-32");
            //BT.write("4100083A8000");
            BT.write("4100181B8001");            
          break;
          case '2':
            Serial.println("PIDs 33-64");
            BT.write("412000000001");            
          break;
          case '4':
            Serial.println("PIDs 65-96");                        
            BT.write("414008000000");            
          break;
          case '6':
            Serial.println("PIDs 97-128");                   
            BT.write("416000000000");
          break;
          case '8':
            Serial.println("PIDs 129-160");            
            BT.write("418000000000");
          break;
        }        
        SendNewLine();
        SendPrompt();
        //SendNewLine();
        ClearBuffer();    
        return true;      
      }
  return false;
}

bool ReceivePIDs()
{   
  if(counter >= 4)
  {
      if(cmds[0] == '0' && cmds[1] == '1' && cmds[3] == '0')
      {
        //Serial.println("Check io");
      }
      else
      {
        Serial.print(cmds[0]);
        Serial.print(cmds[1]);
        Serial.print(cmds[2]);
        Serial.print(cmds[3]);
        Serial.println(" Check Nio");
        return false;
       }
  }
   else
      return false;
      
    
  SendEcho();
  uint8_t elmResponse[12];
  int responseCounter = 0;
  
  if(headerInfo)
  {    
      elmResponse[responseCounter++] = 0x80;                
      elmResponse[responseCounter++] = 0xF1;
      elmResponse[responseCounter++] = 0x11;      
      elmResponse[responseCounter++] = 0x06; //Length
  }
  //ServiceID
  elmResponse[responseCounter++] = 0x41;
  //Last Request
  elmResponse[responseCounter++] = GetByteFromHexString(String(cmds[2]) + String(cmds[3]));
  
  switch(cmds[2])
  {
    case '0':            
      //Engine Coolant Temp | Intake Pressure | RPM | Speed | Intake Air Temp | Throttle | PIDs 33-64
      //ALT
      //00011000000110111000000000000001
      //181F8001     
      //NEU mit gear
      //00011000001110111000000000000101
      //Ignore EngineLoad, AirFlowRate      
//      elmResponse[responseCounter++] = 0x18;
//      elmResponse[responseCounter++] = 0x3B;
//      elmResponse[responseCounter++] = 0x80;
//      elmResponse[responseCounter++] = 0x05;

      elmResponse[responseCounter++] = 0x18;
      elmResponse[responseCounter++] = 0x1B;
      elmResponse[responseCounter++] = 0x80;
      elmResponse[responseCounter++] = 0x01;
      Serial.println("PIDs 01-32");
    break;
    case '2':
      //412000000001      
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x20;
      elmResponse[responseCounter++] = 0x01;
      Serial.println("PIDs 33-64");
    break;
    case '4':
      //414008000000
      //Relative throttle position in %      
      elmResponse[responseCounter++] = 0x48;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x01;
      Serial.println("PIDs 65-96");
    break;
    case '6':            
      //416000000000      
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x02;
      Serial.println("PIDs 97-128");    
    break;
    case '8':
      //418000000000      
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      elmResponse[responseCounter++] = 0x00;
      Serial.println("PIDs 129-160");
    break;
    case 'A':      
      //41A000000000
      if(cmds[3] == '0')
      {        
        elmResponse[responseCounter++] = 0x00;
        elmResponse[responseCounter++] = 0x00;
        elmResponse[responseCounter++] = 0x00;
        elmResponse[responseCounter++] = 0x00;
        Serial.println("PIDs A1 - C0");
      }
    break;
    case 'C':    
      //41C000000000
      if(cmds[3] == '0')
      {        
        elmResponse[responseCounter++] = 0x00;
        elmResponse[responseCounter++] = 0x00;
        elmResponse[responseCounter++] = 0x00;
        elmResponse[responseCounter++] = 0x00;
        Serial.println("PIDs C1 - E0");
      }
    break;
    default:
      Serial.println("UnknownPID List");
    return false;
  }
  //Calc Checksum
  if(headerInfo)   
    elmResponse[responseCounter] = calcChecksum(elmResponse, responseCounter++);  
  for(int i = 0; i < responseCounter; i++)
  {      
    PrintHex(elmResponse[i]);
    if(i +1 < responseCounter)
      SendSpace();
  }

  SendNewLine();
  SendPrompt();  
  Serial.print("PIDs succesful ");
  Serial.println(cmds[2]);
  ClearBuffer();      
  return true;            
}

bool CheckAT()
{
  bool ok = false;
  if(counter >= 2)
      if(cmds[0] == 'A' && cmds[1] == 'T')
      {        
        switch(cmds[2])
        {
          case 'A': //Adaptive Timing
            if(cmds[3] == 'T')
            {
              Serial.print("AdaptiveTiming: ");
              Serial.println(cmds[4] == 1);
              ok = true;
            }
          break;
          case 'D': //Default

            if(cmds[3] == 'P' && cmds[4] == 'N')
            {
              Serial.println("Protocol Number");
              //ISO 14230-4 KWP (fast init, 10.4 kbaud)
              BT.print(5);
              ok = false;
            }//Describe protocol:
            else if(cmds[3] == 'P')
            {
              Serial.println("Describe protocol");
              //ISO 14230-4 KWP (fast init, 10.4 kbaud)
              //BT.print('AUTO, ISO 14230-4 KWP');
              BT.print('ISO 14230-4 KWP');
              ok = false;
            }
            else //Reset settings to Default
            {
              Serial.println("Set to default");
              headerInfo = false;
              spaces = false;
              lineFeed = false;
              echo = false;
              memory = false;
              ok = true;
            }            
          break;
          case 'Z': //Reset
            Serial.println("Reset");
            delay(500);
            BT.print(elmVersion);
            //SendNewLine();
            //ok = true;
          break;         
          case 'H': //Header
            Serial.print("Header: ");
            headerInfo = cmds[3] == '1';
            Serial.println(headerInfo);
            ok = true;
          break;
          case 'I': //Device Info
            Serial.println("DeviceInfo");
            BT.print("ELM327 v1.4");
            ok = false;
          break;
          case 'S': //Space
            if(cmds[3] == 'P')//Protocol Auto
            {
              Serial.println("Protocol Auto");
              ok = true;
            }
            else//Space
            {
              Serial.print("Space: ");
              spaces = cmds[3] == '1';
              Serial.println(spaces);
              ok = true;
            }            
          break;
          case 'L': //lineFeed
            Serial.print("lineFeed: ");
            lineFeed = cmds[3] == '1';
            Serial.println(lineFeed);
            ok = true;
          break;
          case 'E': //Echo
            Serial.print("Echo: ");
            echo = cmds[3] == '1';
            Serial.println(echo);
            ok = true;
          break;
          case 'M': //Memory
          //ToDo:
          //Store last Command for next Startup (Gear :) )
            Serial.print("Memory: ");
            memory = cmds[3] == '1';
            Serial.println(memory);
            ok = true;
          break;   
          case 'P': //ProtocolClose
            if(cmds[3] == 'C')
            {
              Serial.print("Protocol Close");
              ok = true;
            }
            break;
          case 'W': //WarmStart
            if(cmds[3] == 'S')
            {
              Serial.print("WarmStart");
              BT.print(elmVersion);
              ok = false;
            }
          break;       
          case ' ':
            if(cmds[3] == 'A' && cmds[4] == 'T')
            {
              Serial.print("Adaptive Timing: ");
              if(cmds[5] == '0')
                Serial.println("Off");
              if(cmds[5] == '1')
                Serial.println("Auto1");
              if(cmds[5] == '2')
                Serial.println("Auto2");
              ok = true;
            }
            if(cmds[3] == 'H')
            {
              Serial.print("Header (Space): ");
              headerInfo = cmds[4] == '1';
              Serial.println(headerInfo);
              ok = true;
            }
          break;
          case '@':
            switch(cmds[3])
            {
              case '1': //Device Description
                Serial.print("Device Description");
                BT.print("Thomas KDS Reader");
                ok = false;
              break;
              case '2': //Device Identifier
                Serial.print("Device Identifier");
                BT.print("ELM327");
                ok = false;
              break;
              case '3': //Set Device Identifiier (Nope :P )          
                Serial.print("Set Device Identifier");
                ok = true;
              break;
            }            
          break;
          default://Unknown
            Serial.print("Unknown AT Command: ");
            for(int i = 2; i <6;i++)
            {
              Serial.print(cmds[i]);
            }
            Serial.println();
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
        //SendNewLine();
        return true;
      }      
      return false;
}

bool CheckST()
{
  bool ok = false;
  if(counter >= 2)
      if(cmds[0] == 'S' && cmds[1] == 'T')
      {        
        switch(cmds[2])
        {          
          case 'I': //Device Info
            Serial.println("DeviceInfo STI");
            BT.print("ELM327 v1.4");
            ok = false;
          break;         
          default://Unknown
            Serial.print("Unknown ST Command: ");
            for(int i = 2; i <6;i++)
            {
              Serial.print(cmds[i]);
            }
            Serial.println();
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
        //SendNewLine();
        return true;
      }      
      return false;
}

bool CheckService()
{   
    Serial.println("Check Service");
    if(cmds[0] == '0' && cmds[1] == '6' && cmds[2] == '0')
    {
      Serial.println("Service");
    }
    else
    {        
      Serial.println("Service Nio");
      return false;
    }
      
  for(int i = 0; i <= counter; i++)      
        Serial.print(cmds[i]);
  Serial.println();
    
  SendEcho();
  uint8_t elmResponse[12];
  int responseCounter = 0;
  
  if(headerInfo)
  {    
      elmResponse[responseCounter++] = 0x80;                
      elmResponse[responseCounter++] = 0xF1;
      elmResponse[responseCounter++] = 0x11;      
      elmResponse[responseCounter++] = 0x06; //Length
  }
  //ServiceID
  elmResponse[responseCounter++] = 0x41;
  //Last Request
  //elmResponse[responseCounter++] = cmds[2];
  return true;
}

void SendNewLine()
{ 
  //Carriage Return 13
  BT.write(0x0D);
  //LineFeed 10
  if(lineFeed)
    BT.write(0x0A);   
}

void SendPrompt()
{   
   //BT.print(">");
   BT.write(0x3E);
}

void SendSpace()
{
  //if(!spaces) 
  //  BT.print(" "); 
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
    for(int j = 0; j < counter; j++)
    {          
      if(cmds[j] == 0x00)
        break;
      BT.write(cmds[j]);          
    }        
    SendNewLine();
  }
}

void ClearBuffer()
{
  counter = 0;
  memset(cmds, 0, sizeof(cmds));  
}

byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else
     return (byte)(c-'A'+10);
}

byte GetByteFromHexString(String hexValue)
{
  return getVal(hexValue[1]) + (getVal(hexValue[0]) << 4);
}

void PrintHex(uint8_t data) // prints 8-bit data in hex with leading zeroes
{
  if (data < 0x10)         
    BT.print("0");
  String str = String(data,HEX);
  str.toUpperCase();
  BT.print(str);
}

// Checksum is simply the sum of all data bytes modulo 0xFF
// (same as being truncated to one byte)
uint8_t calcChecksum(uint8_t *data, uint8_t len)
{
  uint8_t crc = 0;

  for (uint8_t i = 0; i < len; i++)
  {
    crc = crc + data[i];
  }
  return crc;
}

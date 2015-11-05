#include <SoftwareSerial.h>
SoftwareSerial BT(10, 11); 
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
  //ToDo:
  //Translate chars to HEX
  //While(available) Read:
  char input = BT.read();
  //Ignore Spaces:
  if(input == ' ') 
    return false;
  cmds[counter++] = input;
  //Debug Output
//  Serial.print(counter);
//  Serial.print(": ");
//  Serial.println(input);
  
  //Carriage Return
  if(input == 0x0D)
  {
    //Repeat last command when only \r appears
    if(counter == 1)
      Serial.println("Repeat last Command");
      
    if(CheckAT())
      return true;
    if(ReceivePIDs())
      return true; 
    if(CheckPID())     
      return true;

      //Unknown Command:
      Serial.println("Unknown: ");
      for(int i = 0; i <= counter; i++)      
        Serial.print(cmds[i]);
      Serial.println();
  }
  
  //Linefeed - Ignore (Sometimes sent after CarriageReturn)
  if(input == 0x0A)
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
      default:
        Serial.println("Unknown PID");
        request = 0x00;
      break;
    }

    if(response == "")
    {
      BT.write("NO DATA");    
    }
    else
    {
      //Valid + Sender + ECU-Adress + ? + PID
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

    //Create Test Data
    if(f++ >= 3)
      f = 0;
    return true;    
  }
  return false;
}

bool ReceivePIDs()
{
    if(counter >= 4)
      if(cmds[0] == '0' && cmds[1] == '1' && cmds[3] == '0')      
      { 
        SendEcho();      
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

bool CheckAT()
{
  bool ok = false;
  if(counter >= 2)
      if(cmds[0] == 'A' && cmds[1] == 'T')
      {        
        switch(cmds[2])
        {
          case 'D': //Default
            Serial.println("Set to default");
            headerInfo = false;
            spaces = false;
            lineFeed = false;
            echo = false;
            memory = false;
            ok = true;
          break;
          case 'Z': //Reset
            Serial.println("Reset");
            delay(1000);
            BT.print(elmVersion);
            SendNewLine();
            ok = true;
          break;         
          case 'H': //Header
            Serial.print("Header: ");
            headerInfo = cmds[3] == '1';
            Serial.println(headerInfo);
            ok = true;
          break;
          case 'I': //Device Info
            Serial.println("DeviceInfo");
            BT.print("ELM327");
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
          default://Unknown
            Serial.println("Unknown AT Command!");
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
   BT.print(">");
}

void SendSpace()
{
  if(spaces) 
    BT.print(" "); 
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

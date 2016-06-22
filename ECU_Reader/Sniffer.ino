const uint8_t pidList[] = { 0x00, 0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0};
const uint8_t pidListCount = (uint8_t)(sizeof(pidList));

void SniffEcu()
{
  //PID (0x00 is already in pidList)
  lastPID = 0x01;
  BT.println("Sniffer Started!");
  
  if(!ECUconnected)
    fastInit();

  //Get first PID List
  SniffCommand(pidList[0]);
  
  for (int i = 0; i < pidListCount; i++)
  {    
    //Copy Array  from last PID List before it will be overwritten:
    uint8_t ecuResponseCopy[7];
    memcpy(ecuResponseCopy, ecuResponse, 7);        
    //Result: PID List
    //80 F1 11 06 61 00 DF F7 87 87 CD
    //                  DF F7 87 87
    //1101 1111 1111 0111 1000 0111 1000 0111
    //01 02 XX 04 05 06 07 08 09...
    //For each Hex value in ECUresponse
    for (uint8_t j = 2; j < 6; j++)
    {
      byte bin = ecuResponseCopy[j];
      //0x80 = 1000 0000 | Shift 1 to the right
      for (int mask=0x80; mask != 0; mask >>= 1)
      {        
        if (bin & mask)           
           SniffCommand(lastPID);          
         lastPID++;
      }        
    }
  }
}

void SniffKnownCmds()
{
   BT.println("Known Sniffer Started!");
  
  if(!ECUconnected)
    fastInit();
    
  for(int i = 0; i < 20; i++)
  {
  //Throttle
  SniffCommand(0x04);
  
  //Intake Air Pressure
  //SniffCommand(0x05);
  
  //Enginge Coolant
  //SniffCommand(0x06);
  
  //Intake Air Temp
  //SniffCommand(0x07);  
  
  //Athmospheric Air Pressure - Static about: CB 00
  //SniffCommand(0x08);
  
  //Battery
  //SniffCommand(0x0A);
  
  //Speed
  //SniffCommand(0x0C);
  
  //Sub Throttle
  SniffCommand(0x5B); 
  
  //Internal C2
  //SniffCommand(0x5F); 
  
  //Internal C3 - Static: 00
  //SniffCommand(0x61); 

  //Choke???
  SniffCommand(0x66); 
  
  delay(100);
  //BT.print("-----");
  //BT.println(i);
  }
  PrintThrottleValues();
}

void SniffDiagnostic()
{
  uint8_t cmdBuf[6];
//  cmdBuf[0] = 0x82;
//  cmdBuf[1] = 0x00;
//  cmdBuf[2] = 0x00;
//  cmdBuf[3] = 0x00;
//  cmdBuf[4] = 0x00;
//  cmdBuf[5] = 0x00;
//  
//  //StopCom
//  BT.println("StopCom");
//  SniffCommandList(cmdBuf); 
//  
//  //Stop Routine By Local ID
//  BT.println("StopRoutine");
//  translatedSID = 0x32;
//  SniffCommand(0x81);
//  
//  //Init Ecu Again:
//  fastInit();
  
  //Clear Diagnostic Information
//  BT.println("ClearDiagnostic");    
//  translatedSID = 0x14;
//  //Clears only a special group
//  SniffCommand(0x01);

  //Read Diagnostic Trouble Codes
  BT.println("ReadTroubleCodes");  
  translatedSID = 0x13;
  SniffCommand(0x01);

  //Read Data By Local Id (Error Counter)
  BT.println("ReadErrorCounter");  
  translatedSID = 0x21;
  //PID: Error Counter
  SniffCommand(0x56);
  uint8_t diagCounter = ecuResponse[5];

  //Read Freeze Frame Data
  BT.println("FreezedData"); 
  translatedSID = 0x12;  
  //0x01 0x01 0x54
  //0x02 0x01 0x54
  //0x03 0x01 0x54    
  cmdBuf[0] = translatedSID;
  cmdBuf[2] = 0x01;
  cmdBuf[3] = 0x54;
  cmdBuf[4] = 0x00;
  cmdBuf[5] = 0x00;
  //Test
  diagCounter = 3;
  for(int i = 1; i <= diagCounter;i++)
  {
    cmdBuf[1] = i;
    SniffCommandList(cmdBuf);
  }  
}

void SniffInfo()
{
  uint8_t cmdBuf[6];
  
  BT.println("ECU ID");  
  cmdBuf[0] = 0x1A;
  cmdBuf[1] = 0x00;
  cmdBuf[2] = 0x00;
  cmdBuf[3] = 0x00;
  cmdBuf[4] = 0x00;
  cmdBuf[5] = 0x00;
  SniffCommandList(cmdBuf); 
}

bool SniffCommand(uint8_t pid)
{
  if(ECUconnected)
  {
    BT.print("Out: ");
    PrintHex(pid);
    BT.println();
    if(processRequest(pid))
      //Error (Unknown PID)
      if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
      {
        BT.println("Error!");
        return false;
      }
      else
      {
        BT.print("In: ");
        for (uint8_t i = 0; i < 12; i++)
        {
          PrintHex(ecuResponse[i]);
        }
        BT.println();
        return true;
      }    
  }
  return false;
}
bool SniffCommandList(uint8_t *pids)
{
  uint8_t cmdSize = 0;
  // Zero the response buffer up to maxLen
  for (uint8_t i = 0; i < 12; i++)
  {
    ecuResponse[i] = 0;
  }
  
  if(ECUconnected)    
  {
    BT.print("Out: ");
    //Print and calculate counter
    for(int i = 0; i<6;i++)
      if(pids[i] != 0x00)
      {      
        PrintHex(pids[i]);        
        cmdSize++;
      }
    BT.println();
    if(sendRequest(pids, ecuResponse, cmdSize, 12))
      //Error (Unknown PID)
      if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
      {
        BT.println("Error!");
        return false;
      }
      else
      {
        BT.print("In: ");
        for (uint8_t i = 0; i < 12; i++)
        {
          PrintHex(ecuResponse[i]);
        }
        BT.println();
        return true;
      }
      BT.println("Nix?");    
  }
  return false;
}

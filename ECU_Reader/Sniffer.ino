const uint8_t pidList[] = { 0x00, 0x20, 0x40, 0x60, 0x80};
const uint8_t pidListCount = (uint8_t)(sizeof(pidList));

void SniffEcu()
{
  //PID (0x00 is already in pidList)
  lastPID = 0x01;
  BT.println("Sniffer Started!");
  
  if(!ECUconnected)
    fastInit();
  for (int i = 0; i < pidListCount; i++)
  {
    //Get PID List
    if(SniffCommand(pidList[i]))
    {
      //Copy Array before it will be overwritten:
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
          //(bin & mask) != 0
          if (bin & mask)          
             //ignore next PID-List:
             SniffCommand(lastPID);
          
           lastPID++;
        }        
      }
    }
  }
}

void SniffKnownCmds()
{
   BT.println("Known Sniffer Started!");
  
  if(!ECUconnected)
    fastInit();
    
  for(int i = 0; i < 1; i++)
  {
  //Throttle
  SniffCommand(0x04); 
  
  //Intake Air Pressure
  SniffCommand(0x05);
  
  //Enginge Coolant
  SniffCommand(0x06);
  
  //Intake Air Temp
  SniffCommand(0x07);  
  
  //Athmospheric Air Pressure - Static about: CB 00
  //SniffCommand(0x08);
  
  //Battery
  SniffCommand(0x0A);
  
  //Speed
  //SniffCommand(0x0C);
  
  //Sub Throttle
  SniffCommand(0x5B); 
  
  //Internal C2
  //SniffCommand(0x5F); 
  
  //Internal C3 - Static: 00
  //SniffCommand(0x61); 
  BT.print("-----");
  BT.println(i);
  }
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


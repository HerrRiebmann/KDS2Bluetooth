#include <EEPROM.h>
#include <SoftwareSerial.h>

//-----------------------------//
// ##    Bluetooth HC-06    ## //
//Serial com. on Digital Pin 10 Rx & 11 Tx
//SoftwareSerial BT(10, 11);
SoftwareSerial BT(11, 10);
uint32_t lastBTrequest;
bool bluetoothConnected = false;
const uint8_t BTDelay = 3;
//-----------------------------//

//-----------------------------//
// ##       OBD K-Line      ## //
#define K_OUT 1 // K Output Line - TX (1) on Arduino
#define K_IN 0  // K Input  Line - RX (0) on Arduino
// Timings
#define MAXSENDTIME 2000 // 2 second timeout on KDS comms.
const uint8_t ISORequestByteDelay = 10;
const uint8_t ISORequestDelay = 40; // Time between requests.
// Source and destination adresses, ECU (0x11) & Arduino (0xF1)
const uint8_t ECUaddr = 0x11;
const uint8_t MyAddr = 0xF1;
uint8_t format = 0x81;
bool ECUconnected = false;
uint8_t ecuResponse[12];
uint32_t lastKresponse;
uint16_t ThrottlePosMax = 405; //New 434
uint8_t SubThrottleMax = 189;  //New 196
//-----------------------------//

//-----------------------------//
// ##  ELM327 AT Commands   ## //
bool header = false;
bool spaces = false;
bool linefeed = false;
bool echo = false;
bool memory = false;
const char elmVersion[11] = {'E', 'L', 'M', '3', '2', '7', ' ','v','1','.','4'};
uint8_t lastSID = 0x01;
uint8_t translatedSID = 0x21;
uint8_t lastPID = 0x00;
uint8_t translatedPID = 0x0B;
char elmRequest[23]; //Max.: 80 11 F1 02 21 06 AB \r
int counter = 0;
//-----------------------------//

//Status LED (OnBoard)
#define BOARD_LED 13

char gear = 'N';
uint8_t voltage = 125;

//-----------------------------//
// ##   Bluetooth   Setup   ## //
//-----------------------------//
void SetupBluetooth()
{  
  BT.begin(9600);
  bluetoothConnected = false;
  lastBTrequest = 0;
  //Initialize Bluetooth-Name once
  //BT.print("AT+NAMEThomasZ750r");
  //Stock ELM327 & HC-06 pairing code
  //BT.Print("AT+PIN1234");
}

//-----------------------------//
// ##   OBD 2 K-Line  Setup ## //
//-----------------------------//
void SetupKLine()
{
  // Setup serial pins
  pinMode(K_OUT, OUTPUT);
  pinMode(K_IN, INPUT);
  // define board status led
  pinMode(BOARD_LED, OUTPUT);
  // status led off
  digitalWrite(BOARD_LED, LOW);
  lastKresponse = 0;
  //Read adjusted values
  ThrottlePosMax = EEPROM.read(0) +255;  
  SubThrottleMax = EEPROM.read(1);
  if(ThrottlePosMax == 255 || SubThrottleMax == 0)
    ClearThrottleValues();
}

void setup()
{
  SetupBluetooth();
  SetupKLine();    
}

void loop()
{
  if(BT.available())
    ReadBluetoothInput();
  else
  //Timeout for Serial (BT) to read:  
  delay(BTDelay);

  //Keep alive when device doesn´t
  if(ECUconnected)
  {    
      if(checkIdle())
        keepAlive();
  }  
}

bool checkIdle()
{
    return ((millis() - lastKresponse) > 1600);
}

void keepAlive()
{ 
  if(ECUconnected)
  {
    //ToDo: Replace with "Tester Present"
    //0x3E (SID)
    
    //Check Gear to keep KDS alive
    if(processRequest(0x0B))
      if(ecuResponse[2] == 0x00)
        gear = 'N';
      else    
        gear = String(ecuResponse[2])[0];
      
    //Error (Unconnected)
    if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
    {
      //Error responded        
      ErrorAppeard();        
      return;
    }    
  }
}


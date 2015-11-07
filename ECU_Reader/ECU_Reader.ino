#include <SoftwareSerial.h>

//-----------------------------//
// ##    Bluetooth HC-06    ## //
//Serial com. on Digital Pin 10 Rx & 11 Tx
SoftwareSerial BT(10, 11);
uint32_t lastBTrequest;
bool bluetoothConnected = false;
//-----------------------------//

//-----------------------------//
// ##       OBD K-Line      ## //
#define K_OUT 1 // K Output Line - TX (1) on Arduino
#define K_IN 0  // K Input  Line - RX (0) on Arduino
// Timings
#define MAXSENDTIME 2000 // 2 second timeout on KDS comms.
const uint32_t ISORequestByteDelay = 10;
const uint32_t ISORequestDelay = 40; // Time between requests.
// Source and destination adresses, ECU (0x11) & Arduino (0xF1)
const uint8_t ECUaddr = 0x11;
const uint8_t MyAddr = 0xF1;
bool ECUconnected = false;
uint8_t ecuResponse[12];
uint32_t lastKresponse;
//-----------------------------//

//-----------------------------//
// ##  ELM327 AT Commands   ## //
bool header = false;
bool spaces = false;
bool linefeed = false;
bool echo = false;
bool memory = false;
const char elmVersion[11] = {'E', 'L', 'M', '3', '2', '7', ' ','v','1','.','4'};
uint8_t lastPID = 0x00;
uint8_t translatedPID = 0x0B;
char elmRequest[23]; //Max.: 80 11 F1 02 21 06 AB \r
int counter = 0;
//-----------------------------//

//Status LED (OnBoard)
#define BOARD_LED 13

//-----------------------------//
// ##       LCD 128x64        ## //

//-----------------------------//

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
  // status led aus
  digitalWrite(BOARD_LED, LOW);
  lastKresponse = 0;
}

//-----------------------------//
// ##      LCD     Setup    ## //
//-----------------------------//
void setupLcd()
{
  return;
//  lcd.setCursor(2,0);
//  lcd.print("Kawsaki Z750r");
//  lcd.setCursor(2,1);
//  lcd.print("Black Edition");
//  delay(1000);
//  ClearDisplay(false);
//  lcd.setCursor(0,0);
//  lcd.print("Hallo, Thomas!");
//  delay(1000);
//  ClearDisplay(true);    
//  lcd.print("N123456");
}

void setup()
{
  SetupBluetooth();
  SetupKLine();  
  setupLcd();
}

void loop()
{
  if(BT.available())
    ReadBluetoothInput();
  else
  //Timeout for Serial (BT) to read:
  //Smaller doesn´t seem to work
  //delay(100);
  delay(25);
    
  if (!ECUconnected)
  {
    // fast init sequenz an die ECU senden    
    ECUconnected = fastInit();
    if (ECUconnected)
    {
      // sobald die verbindung zur ECU steht, geht die BOARD LED auf pin13 an, und zeigt den status
      digitalWrite(BOARD_LED, HIGH);
//      lcd.setCursor(13, 1);
//      lcd.print("ECU");
    }    
  }
  
  //When Bluetooth not connected  
  //Or on idle...
  //Or last Response is older then 1 Second
  if(ECUconnected)
  {
    checkBluetooth();
    if(!bluetoothConnected) 
      keepAlive();
    else
      if(checkIdle())
        keepAlive();
  }
  else
  {
    digitalWrite(BOARD_LED, LOW);
//    lcd.setCursor(13, 1);
//    lcd.print("   ");      
  }
}

void checkBluetooth()
{
    if(bluetoothConnected)
      bluetoothConnected = ((millis() - lastBTrequest) < 1000);
//    lcd.setCursor(15, 0);
//    if(bluetoothConnected)      
//      lcd.write((byte)0);
//    else
//    lcd.print(" ");
}

bool checkIdle()
{
    return ((millis() - lastKresponse) > 1600);
}

void keepAlive()
{ 
  if(ECUconnected)
  { 
    String str;
    if(processRequest(0x0B))
      if(ecuResponse[2] == 0x00)
        str = "N";
      else    
        str = String(ecuResponse[2]);

      //Error (Unconnected)
      if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
      {
        //Error responded        
        ErrorAppeard();        
      }
    //ToDo: Voltage, BT Connected, ECU Connected, Enginge Operating Hours, Meter above Sea
    
    //ToDo: Display Stuff...
//    lcd.setCursor(1, 0);
//    lcd.print(str);
  }
}

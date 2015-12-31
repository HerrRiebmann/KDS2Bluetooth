#include <EEPROM.h>
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
const uint8_t ISORequestByteDelay = 10;
const uint8_t ISORequestDelay = 40; // Time between requests.
// Source and destination adresses, ECU (0x11) & Arduino (0xF1)
const uint8_t ECUaddr = 0x11;
const uint8_t MyAddr = 0xF1;
bool ECUconnected = false;
//ToDo: Save only important parts!!!
uint8_t ecuResponse[12];
uint32_t lastKresponse;
uint8_t ThrottlePosMax = 189;
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
//ToDo: Reduce!!!
char elmRequest[23]; //Max.: 80 11 F1 02 21 06 AB \r
int counter = 0;
//-----------------------------//

//Status LED (OnBoard)
#define BOARD_LED 13

//-----------------------------//
// ##       LCD 128x64        ## //
//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//
//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
//-----------------------------//

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

  ThrottlePosMax = EEPROM.read(0);
}

//-----------------------------//
// ##   LCD 128x64 Setup    ## //
//-----------------------------//
void setupLcd()
{
  //Mode: 2IC
  //SDA - A5
  //SCL - A4  
//  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
//  //China Clone :)
//  //display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
//  // init done
//  
//  
//  // Show image buffer on the display hardware.
//  // Since the buffer is intialized with an Adafruit splashscreen
//  // internally, this will display the splashscreen.
//  display.clearDisplay();
//  //ToDo: overwrite Splashscreen with Kawasaki Logo!
//  //display.display();
//  //delay(500);
//
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0,0);
//  display.println(" Kawasaki");
//  display.println("   Z750r");
//  display.setTextColor(BLACK, WHITE); // 'inverted' text
//  display.println("  Black  ");  
//  display.println(" Edition ");  
//  display.display();
//  delay(2000);
//  display.clearDisplay();
//  display.display();
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
    }    
  }
  
  //When Bluetooth not connected  
  //Or on idle...
  //Or last Response is older then 1 Second
  if(ECUconnected)
  {    
    if(!checkBluetooth()) 
      keepAlive();
    else
      if(checkIdle())
        keepAlive();
  }
  else
  {
    digitalWrite(BOARD_LED, LOW);
  }
}

bool checkBluetooth()
{
    if(bluetoothConnected)
      bluetoothConnected = ((millis() - lastBTrequest) < 1000);
    return bluetoothConnected;
}

bool checkIdle()
{
    return ((millis() - lastKresponse) > 1600);
}

void keepAlive()
{ 
  if(ECUconnected)
  {     
    //Gear
    if(processRequest(0x0B))
      if(ecuResponse[2] == 0x00)
        gear = 'N';
      else    
        gear = String(ecuResponse[2])[0];
        
    //Voltage    
//    if(processRequest(0x0A))
//      voltage = ecuResponse[2];
      
    //Error (Unconnected)
    if(ecuResponse[0] == 0x7F && ecuResponse[1] == 0x21 && ecuResponse[2] == 0x10)
    {
      //Error responded        
      ErrorAppeard();        
      return;
    }
    //ToDo: Voltage, BT Connected, ECU Connected, Engine Operating Hours, Meter above Sea
    
    //ToDo: Display Stuff...
    drawDisplay();
  }
}


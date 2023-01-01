#define NewBoard 0
const float FWVersion = 1.17;
#if NewBoard
const float BoardVersion = 1.30;
#else
const float BoardVersion = 1.00;
#endif

//FW 1.01: Logger optimization & Gear-Test
//FW 1.02: Advance & Gear transmission
//FW 1.03: Cleanup (Size)
//FW 1.04: Function for testing
//FW 1.05: Code Cleanup
//FW 1.06: Gearratio above 20kmh & 2000rpm
//FW 1.07: Long BT-Commands didn´t work (Timeout), Lowercase on BT, Error printout chars
//FW 1.08: Added functionality to fetch data from a working communication
//FW 1.09: Kawasaki ABS
//FW 1.10: Bluetooth initialization (for HC-06-20190901)
//FW 1.11: K-Line Buffer exceeded (wrong check) & sized to 133, don´t print via BT when debugMode active, code cleanup, Transmission Current Gear & Enginge Runtime, Reduced Timings
//FW 1.12: Kawa DTC, Yamaha, FetchMode enhancement
//FW 1.13: Suzuki Factor to 256, optimized test-function (XTXi)
//FW 1.14: Read all freezed frames (Kawa only)
//FW 1.15: ELM327:1.4 -> 1.5 (For SZ Viewer A1 compatibility)
//FW 1.16: Revert back to ELM327:1.4, Added ATKW & ATCAF (Several CAN-Bus configs), Optimized FetchData()
//FW 1.17: Sniffer/Fetch-mode optimizations 

//_**  Libraries   **_//
#include <EEPROM.h>
#include <SoftwareSerial.h>

//_**  EEPROM Storage   **_//
#define EEPROM_Offset 1
#define EEPROM_Error_Counter 2

#define EEPROM_BTVersion 10
#define EEPROM_BikeProtocol 11
#define EEPROM_RawMode 12
#define EEPROM_TemporaryStorage 13
#define EEPROM_AutoGear 14

#define EEPROM_ThrottleMin 20
#define EEPROM_ThrottleMaxLow 21
#define EEPROM_ThrottleMaxHigh 22
#define EEPROM_SubThrottleMin 25
#define EEPROM_SubThrottleMax 26

#define EEPROM_Error_ID 40
#define EEPROM_Error_SID 41
#define EEPROM_Error_PID 42
#define EEPROM_Error_7F 43
#define EEPROM_Error_Min 44  //Minutes since start

//_**  Error Types  **_//
#define ERROR_StartCom 1
#define ERROR_Checksum 2
#define ERROR_Receive_Timeout 3
#define ERROR_7F_Response 4
#define ERROR_Unknown_SID 5
#define ERROR_Unknown_PID 6
#define ERROR_BT_Timeout 7
#define ERROR_Unkonwn_InitProtocol 8
#define ERROR_7F_Response_KA 9
#define ERROR_FastInit 10
#define ERROR_SlowInit 11
#define ERROR_StopCom 12
#define ERROR_BT_VersionNotFound 13
#define ERROR_BT_BaudNotFound 14
#define ERROR_BT_BaudNotChanged 15
#define ERROR_BT_WrongVersionInEeprom 16
#define ERROR_ELMnoData 20
#define ERROR_Unknown_AT 21
#define ERROR_Unknown_XT 22
#define ERROR_Unknown_CMD 23
#define ERROR_BT_Buffer_Exeeded 24
#define ERROR_K_Buffer_Exeeded 25

//_**  Bluetooth Interface   **_//
#define BT_Timeout 500  // 0,5 second timeout on Wireless commands
#define BT_Delay 3      // 3 ms delay between reads if nothing available
#define BT_Baud 57600
#define BT_BaudFallback 9600
#define BT_Cmd_Delay 2000 //Previously 1000, but since new BT module 2sec

#define BUFFER_SIZE 133         //Suzuki PID 0x80 length 102 + header & checksum (should be max 128 for data + 5 for header & checksum)
uint8_t btBuffer[BUFFER_SIZE];  //Incoming hex from Bluetooth
uint8_t btBufferCounter = 0;
uint8_t btIncomingCounter = 0;
uint8_t hexBuffer = 0xFF;
long baud = 0;

SoftwareSerial BT(8, 7);

uint32_t lastBtRequest;
bool IsHexRequest = false;
uint8_t btVersion = 0xFF;

//_**     K-Line    ## //
#define K_IN 0                   // K Input  Line - RX (0) on Arduino
#define K_OUT 1                  // K Output Line - TX (1) on Arduino
#define K_MaxSendTime 2000       // 2 second timeout on KDS commands
#define K_ErrorTimeout 1000      // 500 minimum //-> Max value 5 seconds timeout before reinitialization. (Minimum 55?)
#define K_Baud 10400             // Should be 10417
#define K_Keepalive 1500         // 1,5 seconds no request, send keepalive (2s is ECU timeout)
#define K_ISORequestDelay 55     // 55 MS between requests. (25 MS between two requests, 55 MS after response)
#define K_ISORequestByteDelay 5  // 5 MS between single bytes.

bool ecuConnected = false;
uint32_t lastKlineResponse = 0;
uint8_t customFormat = 0xFF;
uint8_t ecuAddress = 0x11;  // 0x11 & 0x28 Kawasaki (ECU & ABS), 0x12 Suzuki, 0x33 Yamaha
uint8_t myAddress = 0xF1;
uint8_t bikeProtocol = 0;  // Kawa, KawaABS, Suzuki1, Suzuki2, Honda1, Honda2, (MV Augusta Brutale - Should be possible), Yamaha
uint8_t keepaliveMsg[3] = { 0x3E, 0xFF, 0xFF };

uint8_t ecuBufferIn[BUFFER_SIZE];  //Incoming hex from ECU
uint8_t ecuBufferInCounter = 0;
uint8_t ecuBufferOutTmp[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  //Temporary outgoing hex to ECU for individual messages
uint8_t ecuBufferOutTmpCounter = 0;

//_**   Throttle Percentages   **_//
uint8_t throttlePosMinimum = 201;   //Kawa 201, Suzi 39
uint16_t throttlePosMaximum = 892;  //Kawa 892, Suzi 221
uint8_t subThrottleMinimum = 81;    //Kawa 81,  Suzi 26
uint8_t subThrottleMaximum = 189;   //Kawa 189, Suzi 185

//_**   ELM Setup   **_//
bool header = false;
bool spaces = false;
bool linefeed = false;
bool echo = false;
bool memory = false;
uint8_t adaptiveTiming = 0;
uint8_t isoProtocol = 5;  //5: ISO 14230-4 KWP (fast init, 10.4 kbaud)
const char elmVersion[] = "ELM327 v1.4";

//_**   Custom Setup   **_//
bool sendKeepAlive = true;
bool debugMode = false;
bool fetchMode = false;
bool tempStorage = true;
bool autoGear = false;
bool rawMode = false;
bool bypassInit = false;
bool defaultCheckSum = true;
uint16_t Factor = 255;
uint8_t eepromOffset = 0;
uint8_t errorCounter = 0;
uint32_t lastDataUpdate = 0;

#define LED_STATUS A0  //Board V2

void setup() {
  InitializeStatusLed();
  SetStatusLed(HIGH);

  InitializeKLine();

  InitializeStorage();
  
  InitializeBluetooth();

  InitializeManufacturer();

  SetStatusLed(LOW);
}

void loop() {
  ReadAvailableInput();

  // check bluetooth timeout:
  CheckInputTimeout();

  // send keepalive message
  if (CheckIdle())
    StayAlive();
}

void InitializeStatusLed() {
  pinMode(LED_STATUS, OUTPUT);
}

void SetStatusLed(const bool state) {
  digitalWrite(LED_STATUS, !state);
}

String GetHex(uint8_t data) {
  String str;
  if (data < 0x10)
    str = "0";
  str.concat(String(data, HEX));
  str.toUpperCase();
  return str;
}

byte GetByteFromHexValues(const uint8_t value1, const uint8_t value2) {
  return GetVal(value2) + (GetVal(value1) << 4);
}

byte GetVal(const char c) {
  if (c >= '0' && c <= '9')
    return (byte)(c - '0');
  else
    return (byte)(c - 'A' + 10);
}

bool CheckIsHexCommand(uint8_t hexBuffer1, uint8_t hexBuffer2) {
  return ((hexBuffer1 >= '0' && hexBuffer1 <= '9') || (hexBuffer1 >= 'A' && hexBuffer1 <= 'F')) && ((hexBuffer2 >= '0' && hexBuffer2 <= '9') || (hexBuffer2 >= 'A' && hexBuffer2 <= 'F'));
}

void SetCharToUpper(char &input) {
  //Except Bluetooth Command:
  if (btBufferCounter >= 2 && btBuffer[0] == 'B' && btBuffer[1] == 'T')
    return;
  if (input >= 'a' && input <= 'z')
    input -= 32;
}

bool Compare(const char *str) {
  uint8_t len = strlen(str);
  if (btBufferCounter != len)
    return false;
  for (uint8_t i = 0; i < len; i++) {
    if (btBuffer[i] == '\0')
      return false;
    switch (str[i]) {
      case 'b':  //Boolean placeholder
        if (!(btBuffer[i] == '0' || btBuffer[i] == '1'))
          return false;
        break;
      case 'h':  //Hexadecimal placeholder
        if (!((btBuffer[i] >= '0' || btBuffer[i] <= '9') || (btBuffer[i] >= 'A' || btBuffer[i] <= 'F')))
          return false;
        break;
      case 'i':  //Integer placeholder
        if (btBuffer[i] < '0' || btBuffer[i] > '9')
          return false;
        break;
      default:  //Command char(s)
        if (btBuffer[i] != str[i])
          return false;
        break;
    }
  }
  return true;
}

uint16_t CalculateValue(float resolutionFactor) {
  return CalculateValue(resolutionFactor, 0);
}

uint16_t CalculateValue(float resolutionFactor, uint8_t offset) {
  uint16_t result = 0;
  uint8_t msgLength = 1;
  uint8_t startingPoint = 6;
  if (ecuBufferIn[0] == 0x80)
    msgLength = ecuBufferIn[3] - 2;  //Skip SID & PID
  else                               //Without Length (0x81)
    startingPoint = 3;

  for (uint8_t i = 0; i < msgLength; i++) {
    result += ecuBufferIn[startingPoint + i];
    if (i + 1 < msgLength)
      result *= 256;  //A + B allways *256. Independend of 255 or 256 calculation factor
  }
  //No need for calculation
  if (resolutionFactor == Factor)
    return result - offset;
  return ((result * resolutionFactor) / Factor) - offset;
}

uint8_t CalcGearToEngineLoad(uint8_t gear) {
  return map(gear, 0, 6, 0, 15);  // Value / 2,55 (15/2,55 == 5,9)
}

long readVcc() {
  long result; // Read 1.1V reference against AVcc 
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC);  // Convert 
  while (bit_is_set(ADCSRA,ADSC)); //mesure
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

long readTemp() {
  long result; // Read temperature sensor against 1.1V reference 
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); //mesure
  result = ADCL;
  result |= ADCH<<8;
  result = (result - 125) * 1075;
  return result;
}

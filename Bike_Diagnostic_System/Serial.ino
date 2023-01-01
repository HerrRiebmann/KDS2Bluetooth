void StartSerial() {
  Serial.begin(K_Baud);
  ClearSerialBuffer();
}

void EndSerial() {
  ClearSerialBuffer();
  Serial.end();
}

void ClearSerialBuffer() {
  while (Serial.available())
    Serial.read();
}

void SetKline(bool state) {
  digitalWrite(K_OUT, state);
}

void TestKline() {
  //ToDo: Delete after Oszi-Checks  
//  uint8_t delayms = 1;
//  SetStatusLed(HIGH);
//  EndSerial();
//  SetKline(true);
//  delay(delayms);
//  SetKline(false);
//  delay(delayms);
//  SetKline(true);
//  delay(delayms);
//  SetKline(false);
//  delay(delayms);
//  StartSerial();
//  Serial.print(F("Test"));
//  SetStatusLed(LOW);  
}

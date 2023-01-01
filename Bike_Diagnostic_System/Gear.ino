uint8_t GetCurrentGear() {
  //Neutral or Engine off
  if (data.Speed == 0 || data.Speed == 0xFF || data.RPM[0] == 0xFF || data.RPM[1] == 0xFF)
    return 0;

  uint16_t rpm = data.RPM[0] * 64; //*256 / 4 (OBD)
  if (data.RPM[1] >= 4)
    rpm += data.RPM[1] / 4;
  uint8_t ratio = rpm / data.Speed;

  switch(ratio){
    case 130 ... 180:
      return 1;
    case  88 ... 120:
      return 2;
    case  70 ... 80:
      return 3;
    case  60 ... 69:
      return 4;
    case  55 ... 59:
      return 5;
    case  48 ... 54:
      return 6;
    default:
      //If ratio not exact and bike not rolling out, return last valid gear
      if(data.Speed > 20 && rpm > 2000)
        return data.TransmissionGear;
      return 0;
  }
}

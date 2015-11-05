//void ClearDisplay(bool toLeft)
//{
//  for (int positionCounter = 0; positionCounter < 16; positionCounter++)
//  {
//    // scroll one position left:
//    if(toLeft)
//      lcd.scrollDisplayLeft();
//    else
//      lcd.scrollDisplayRight();
//    lcd.setCursor(positionCounter,0);
//    lcd.print(" ");
//    lcd.setCursor(positionCounter,1);
//    lcd.print(" ");
//    // wait a bit:
//    delay(150);
//  }  
//  for (int positionCounter = 0; positionCounter < 16; positionCounter++)
//  {    
//    if(toLeft)
//      lcd.scrollDisplayRight();
//    else
//      lcd.scrollDisplayLeft();
//  }
//}


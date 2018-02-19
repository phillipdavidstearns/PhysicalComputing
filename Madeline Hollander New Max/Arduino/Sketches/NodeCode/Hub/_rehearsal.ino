void rehearsal() { // state 7 STATE_REHEARSAL
  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        break;
      case BUTTON_DOWN:
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        compCount = 0;
        testTemp = 65;
        maxReached = false;
        theEnd = false;
        theStart = true;
        state = STATE_READY; // readyPrompt()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  rehearsalLogic();

  lcd.clear();
  lcd.print("C:");
  lcd.print(compCount+1);
  lcd.print(" T:");
  lcd.print(testTemp);
  lcd.print(" A:");
  lcd.print(maxReached);
  lcd.setCursor(0, 1);
  lcd.print("MIN:");
  lcd.print(int(minTemp));
  lcd.print(" MAX:");
  lcd.print(int(maxTemp));
}

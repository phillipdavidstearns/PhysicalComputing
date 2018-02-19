void setCycles() { //STATE_SET_CYCLES

  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        state = STATE_SET_STARTTEMP;
        break;
      case BUTTON_UP:
        if (compCycles < 24) compCycles++;
        break;
      case BUTTON_DOWN:
        if (compCycles > 1) compCycles--;
        break;
      case BUTTON_LEFT:
        state = STATE_SET_INCREMENT;
        break;
      case BUTTON_SELECT:
        createComposition();
        state = STATE_READY;
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("COMP SETUP");
  lcd.setCursor(0, 1);
  lcd.print("#CYCLES: ");
  lcd.print(compCycles);

} 

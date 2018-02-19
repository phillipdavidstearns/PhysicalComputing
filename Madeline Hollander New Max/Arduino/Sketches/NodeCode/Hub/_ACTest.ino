void ACTest() { // STATE_AC_TEST
  
  play = false;

  button = ReadButtons();
  
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        AC = true;
        break;
      case BUTTON_DOWN:
        AC = false;
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        setAC(0);
        state = STATE_TEMP_STATS;// tempStats()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("Testing AC:");
  lcd.setCursor(0, 1);

  if (AC) {
    setAC(1);
    lcd.print("ON  ");
  } else {
    setAC(0);
    lcd.print("OFF");
  }

}

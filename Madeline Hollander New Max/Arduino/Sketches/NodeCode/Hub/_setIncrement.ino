void setIncrement() { //STATE_SET_INCREMENT

  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        state = STATE_SET_CYCLES;
        break;
      case BUTTON_UP:
        increment++;
        if (spread < increment) spread = increment;
        break;
      case BUTTON_DOWN:
        if (increment > 0) increment--;
        break;
      case BUTTON_LEFT:
        state = STATE_SET_SPREAD;
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
  lcd.print("TEMP INC: ");
  lcd.print(increment);

}

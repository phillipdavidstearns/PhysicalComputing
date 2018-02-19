void setStartTemp() { // STATE_SET_STARTTEMP

  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        state = STATE_SET_SPREAD;
        break;
      case BUTTON_UP:
        if (startTemp < 100) startTemp++;
        break;
      case BUTTON_DOWN:
        if (startTemp > 60) startTemp--;
        break;
      case BUTTON_LEFT:
        state = STATE_SET_CYCLES;
        break;
      case BUTTON_SELECT:
        createComposition();
        state = STATE_READY; // readyPrompt()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("COMP SETUP");
  lcd.setCursor(0, 1);
  lcd.print("START TEMP: ");
  lcd.print(startTemp);

}

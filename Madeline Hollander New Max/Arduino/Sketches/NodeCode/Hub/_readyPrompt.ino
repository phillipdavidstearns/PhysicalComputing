void readyPrompt() { // STATE_READY

  play = false;

  // Button Bindings
  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        state = STATE_REHEARSAL; // rehearsal()
        break;
      case BUTTON_DOWN:
        state = STATE_SET_STARTTEMP;
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        state = STATE_COMP_STATS; // compStats()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  // Display
  lcd.clear();
  lcd.print("Ready?");
  lcd.setCursor(0, 1);
  lcd.print("Avg Temp: ");
  lcd.print(avgTemp);

  if (lastState == 0 || lastState == STATE_END) {
    setAC(0);
    setLights(255);
  }

}

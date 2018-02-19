void lightStats() { // STATE_COMP_STATS

  play = true;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        state = STATE_COMP_STATS; // tempStats()
        break;
      case BUTTON_DOWN:
        state = STATE_TEMP_STATS; // lightStats()
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        state = STATE_LIGHT_CAL; // compPosition()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("BRIGHTNESS: ");
  lcd.print(calculateBrightness(avgTemp));
  lcd.setCursor(0, 1);
  lcd.print("%: ");
  lcd.print(100*percentage);
}

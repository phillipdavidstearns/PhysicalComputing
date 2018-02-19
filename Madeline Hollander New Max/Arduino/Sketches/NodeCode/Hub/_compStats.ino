void compStats() { // STATE_COMP_STATS

  play = true;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        state = STATE_TEMP_STATS; // tempStats()
        break;
      case BUTTON_DOWN:
        state = STATE_LIGHT_STATS; // lightStats()
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        oldCount = compCount;
        newCount = compCount;
        state = STATE_COMP_POS; // compPosition()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("C:");
  lcd.print(compCount+1);
  lcd.print(" T:");
  lcd.print(avgTemp);
  lcd.print(" A:");
  lcd.print(maxReached);
  lcd.setCursor(0, 1);
  lcd.print("MIN:");
  lcd.print(int(minTemp));
  lcd.print(" MAX:");
  lcd.print(int(maxTemp));
}

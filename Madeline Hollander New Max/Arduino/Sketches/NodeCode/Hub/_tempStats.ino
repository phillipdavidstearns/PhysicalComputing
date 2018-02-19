void tempStats() {// STATE_TEMP_STATS

  play = true;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        state = STATE_LIGHT_STATS; // lightStats()
        break;
      case BUTTON_DOWN:
        state = STATE_COMP_STATS; // compStats()
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        state = STATE_AC_TEST;// ACTest()
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("T1:");
  lcd.print(tempLast[0]);
  lcd.print(" T2:");
  lcd.print(tempLast[1]);
  lcd.setCursor(0, 1);
  lcd.print("T3:");
  lcd.print(tempLast[2]);
  lcd.print(" T4:");
  lcd.print(tempLast[3]);
}

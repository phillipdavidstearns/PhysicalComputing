void setOffsetTemp() { //STATE_SET_OFFSETTEMP

  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        state = STATE_COMP_POS;
        break;
      case BUTTON_UP:
        if (offsetTemp < 20) offsetTemp++;
        break;
      case BUTTON_DOWN:
        if (offsetTemp > -20) offsetTemp--;
        break;
      case BUTTON_LEFT:
        break;
      case BUTTON_SELECT:
        maxReached = false;
        updateMinMaxTemp();
        state = STATE_COMP_POS;
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("TEMPERATURE");
  lcd.setCursor(0, 1);
  lcd.print("OFFSET: ");
  lcd.print(offsetTemp);
}

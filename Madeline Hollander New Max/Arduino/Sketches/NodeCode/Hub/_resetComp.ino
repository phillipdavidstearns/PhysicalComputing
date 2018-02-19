void resetComp() { // STATE_RESET

  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        break;
      case BUTTON_UP:
        reset = true;
        break;
      case BUTTON_DOWN:
        reset = false;
        break;
      case BUTTON_LEFT:
        state = STATE_COMP_POS;
        break;
      case BUTTON_SELECT:
        if (reset) {
          maxReached = false;
          theEnd = false;
          theStart = true;
          compCount = 0;
          state = STATE_COMP_POS;
        }
        

        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("RESET PIECE?");
  lcd.setCursor(0, 1);
  if (reset) {
    lcd.print("Y");
  } else {
    lcd.print("N");
  }
}

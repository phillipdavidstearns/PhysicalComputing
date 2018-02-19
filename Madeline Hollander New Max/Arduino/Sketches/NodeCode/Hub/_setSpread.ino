void setSpread() { //STATE_SET_SPREAD

  play = false;

  button = ReadButtons();
  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        state = STATE_SET_INCREMENT;
        break;
      case BUTTON_UP:
        if (spread < 10) spread++;
        break;
      case BUTTON_DOWN:
        if (spread > 1) spread--;
        if (increment > spread){
         increment = spread;
        }
        break;
      case BUTTON_LEFT:
        state = STATE_SET_STARTTEMP;
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
  lcd.print("TEMP SPREAD: ");
  lcd.print(spread);

} 

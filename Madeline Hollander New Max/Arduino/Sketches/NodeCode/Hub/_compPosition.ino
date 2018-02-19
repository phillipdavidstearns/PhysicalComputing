void compPosition() { // STATE_COMP_POS

  play = false;

  button = ReadButtons();

  if ( buttonJustPressed /*|| buttonJustReleased*/) {
    switch ( button ) {
      case BUTTON_NONE:
        break;
      case BUTTON_RIGHT:
        state = STATE_RESET; // resetComp()
        break;
      case BUTTON_UP:
        if (newCount < compCycles) newCount++;
        break;
      case BUTTON_DOWN:
        if (newCount > 0) newCount--;
        break;
      case BUTTON_LEFT:
        state = STATE_SET_OFFSETTEMP; // setOffsetTemp()
        break;
      case BUTTON_SELECT:
      
        maxReached = false;
        
        if (oldCount != newCount) {
          compCount = newCount;
          if(compCount == 0){
            theStart = true;
            theEnd = false;
          } else if (compCount == compCycles){
            theStart = false;
            theEnd = true;
          } else {
            theStart = false;
            theEnd = false;
          }
        }
        
        state = STATE_COMP_STATS; // compStats()
        
        break;
      default:
        break;
    }
  }
  clearButtonFlags();

  lcd.clear();
  lcd.print("Old Count: ");
  lcd.print(oldCount+1);
  lcd.setCursor(0, 1);
  lcd.print("Change to: ");
  lcd.print(newCount+1);

}

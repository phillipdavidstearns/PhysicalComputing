void ending() {

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
          break;
        case BUTTON_DOWN:
          break;
        case BUTTON_LEFT:
          break;
        case BUTTON_SELECT:
          maxReached = false;
          theEnd = false;
          theStart = true;
          compCount = 0;
          state = STATE_READY; // compStats()
          break;
        default:
          break;
      }
    }
    clearButtonFlags();

    // Display
    lcd.clear();
    lcd.print("      THE");
    lcd.setCursor(0, 1);
    lcd.print("      END");

    setAC(0);
    setLights(0);

}


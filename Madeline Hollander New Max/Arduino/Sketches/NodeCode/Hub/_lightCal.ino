void lightCal() { // state 4

  play = false;

  byte button;

  lcd.clear();
  lcd.print("SET LEVEL: ");
  lcd.setCursor(0, 1);

  button = ReadButtons();



  if (lightMode) {
    if ( buttonJustPressed /*|| buttonJustReleased*/) {
      switch ( button ) {
        case BUTTON_NONE:
          break;
        case BUTTON_RIGHT:
          storeBrightness = 255;
          break;
        case BUTTON_UP:
          if (maxBrightness < 255) maxBrightness++;
          storeBrightness = maxBrightness;
          break;
        case BUTTON_DOWN:
          if (maxBrightness > 0) maxBrightness--;
          if (maxBrightness < minBrightness ) minBrightness = maxBrightness;
          storeBrightness = maxBrightness;
          break;
        case BUTTON_LEFT:
          if (storeBrightness == 255) {
            if (maxBrightness == 255) {
              storeBrightness = minBrightness;
              lightMode = !lightMode;
            } else {
              storeBrightness = maxBrightness;
            }
          } else {
            storeBrightness = minBrightness;
            lightMode = !lightMode;
          }
          break;
        case BUTTON_SELECT:
          state = STATE_LIGHT_STATS;// lightStats()
          break;
        default:
          break;
      }

      if (storeBrightness == 255 && maxBrightness != 255) {
        lcd.print("FULL ON: ");
        lcd.print(storeBrightness);
      } else {
        lcd.print("MAX BRIGHT: ");
        lcd.print("   ");
        lcd.setCursor(12, 1);
        lcd.print(storeBrightness);
      }
    }
    clearButtonFlags();
  } else {
    if ( buttonJustPressed /*|| buttonJustReleased*/) {
      switch ( button ) {
        case BUTTON_NONE:
          break;
        case BUTTON_RIGHT:
          if (storeBrightness == 0) {
            if (minBrightness == 0) {
              storeBrightness = maxBrightness;
              lightMode = !lightMode;
            } else {
              storeBrightness = minBrightness;
            }
          } else {
            storeBrightness = maxBrightness;
            lightMode = !lightMode;
          }
          break;
        case BUTTON_UP:
          if (minBrightness < 255)minBrightness++;
          if (minBrightness > maxBrightness) maxBrightness = minBrightness;
          storeBrightness = minBrightness;
          break;
        case BUTTON_DOWN:
          if (minBrightness > 0) minBrightness--;
          storeBrightness = minBrightness;
          break;
        case BUTTON_LEFT:
          storeBrightness = 0;
          break;
        case BUTTON_SELECT:
          state = STATE_LIGHT_STATS;// lightStats()
          break;
        default:
          break;
      }

      if (storeBrightness == 0 && maxBrightness != 0) {
        lcd.print("FULL OFF: ");
        lcd.print(storeBrightness);
      } else {
        lcd.print("MIN BRIGHT: ");
        lcd.print("   ");
        lcd.setCursor(12, 1);
        lcd.print(storeBrightness);
      }
    }
    clearButtonFlags();
  }


  setLights(storeBrightness); // send to the DimmerNode
}

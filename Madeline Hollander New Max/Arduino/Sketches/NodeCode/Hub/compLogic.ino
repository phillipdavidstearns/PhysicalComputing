/********************************
    Composition Logic - Controls the flow of the composition
********************************/
void compLogic() {

  theEnd = compCount >= compCycles;

  if (theEnd) {
    if (avgTemp >= startTemp + offsetTemp) {
      maxReached = true; //turn on AC
    } else {
      maxReached = false; // when startTemp is hit, turn off AC
      state = STATE_END;
    }
  } else {

    updateMinMaxTemp();

    if (theStart) {
      if (avgTemp >= startTemp + offsetTemp) {
        maxReached = true; //turn on AC
      } else {
        maxReached = false; //run it until it's down to 65 degrees
        theStart = false;
      }
    } else {
      if (!maxReached) {
        if (avgTemp >= maxTemp) {
          maxReached = true; //turn on the AC
          if(!theEnd)compCount++;
        }
      } else {
        if (avgTemp <= minTemp) {
          maxReached = false; //turn it off once we're back down to the new minimum
        }
      }
    }
  }

  if (!theEnd) {
    if (maxReached) {
      setLights(0);
    } else {
      setLights(calculateBrightness(avgTemp));
    }
  } else {
    setLights(0);
  }

  setAC(maxReached);

}

/********************************
    Rehearsal Logic - simulates the flow of the composition, can be used to test and adjust composition logic.
********************************/

void rehearsalLogic() {

  testTemp += (incTemp * dirTemp);

  theEnd = compCount >= compCycles ;

  if (theEnd) {
    if (testTemp >= startTemp + offsetTemp) {
      maxReached = true; //turn on AC
      dirTemp = -1;
    } else {
      maxReached = false; //run it until it's down to 65 degrees
      state = STATE_END;
      dirTemp = 0;
    }
  } else {
    
    updateMinMaxTemp();
    
    if (theStart) {
      if (testTemp >= startTemp + offsetTemp) {
        maxReached = true; //turn on AC
        dirTemp = -1;
      } else {
        maxReached = false; //run it until it's down to 65 degrees
        theStart = false;
        dirTemp = 1;
      }
    } else {
      if (!maxReached) {
        if (testTemp >= maxTemp) {
          maxReached = true; //turn on the AC
          compCount++;
          dirTemp = -1;
        }
      } else {
        if (testTemp <= minTemp) {
          maxReached = false; //turn it off once we're back down to the new minimum
          dirTemp = 1;
        }
      }
    }
  }

  if (!theEnd) {
    if (maxReached) {
      setLights(0);
    } else {
      setLights(calculateBrightness(testTemp));
    }
  } else {
    setLights(0);
  }

  setAC(maxReached);

}

void createComposition() {
  for (int i = 0 ; i < compCycles; i++) {
//    Serial.print(i);
//    Serial.print(" - Min & Max: ");
    for (int j = 0 ; j < 2 ; j++) {
      composition[i][j] = startTemp + (i * increment) + (j * spread);
//      Serial.print(composition[i][j]);
//      Serial.print("  ");
    }
//    Serial.println();
  }
}


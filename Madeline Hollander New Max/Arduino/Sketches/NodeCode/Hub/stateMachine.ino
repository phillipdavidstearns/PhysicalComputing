// State machine for structuring the interface and interaction


void stateMachine() {

  updateState(); // helper to track what the lastState our state machine was in

  if (play) compLogic(); // only run the composition logic if play is true.

  switch (state) {

    case STATE_READY:
      readyPrompt();
      break;
    case STATE_COMP_STATS:
      compStats();
      break;
    case STATE_COMP_POS:
      compPosition();
      break;
    case STATE_LIGHT_STATS:
      lightStats();
      break;
    case STATE_LIGHT_CAL:
      lightCal();
      break;
    case STATE_TEMP_STATS:
      tempStats();
      break;
    case STATE_AC_TEST:
      ACTest();
      break;
    case STATE_REHEARSAL:
      rehearsal();
      break;
    case STATE_SET_OFFSETTEMP:
      setOffsetTemp();
      break;
    case STATE_RESET:
      resetComp();
      break;
    case STATE_SET_STARTTEMP:
      setStartTemp();
      break;
    case STATE_SET_SPREAD:
      setSpread();
      break;
    case STATE_SET_INCREMENT:
      setIncrement();
      break;
    case STATE_SET_CYCLES:
      setCycles();
      break;
      case STATE_END:
      ending();
      break;
    default:
      break;
  } // end switch()
} // end program()


void updateState() { // used to keep track of the last state
  if (thisState != state) {
    lastState = thisState;
    thisState = state;
  }
}


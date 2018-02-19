boolean setAC(byte _data) {
  boolean _state;
  byte set = _data;
  radio.stopListening();
  radio.openWritingPipe(rAddress[4]);  // AC units are on pipe 4
  _state = radio.write(&set, 1);
  radio.startListening();
  return _state;
}

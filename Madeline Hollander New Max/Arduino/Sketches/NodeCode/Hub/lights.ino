boolean setLights(byte _data) {
  boolean _state = false;
  byte set = _data;
  radio.stopListening();
  radio.openWritingPipe(rAddress[5]); //lights are on pipe 5
  _state = radio.write(&set, 1);
  radio.startListening();
  return _state;
}

byte calculateBrightness(float _temp) {

  percentage = 1.0 - ( (maxTemp - _temp) / (maxTemp - minTemp) );
  if (percentage > 1.0) percentage = 1.0;
  if (percentage < 0.0) percentage = 0.0;

  byte _brightness = int((maxBrightness - minBrightness) * percentage) + minBrightness;

  return _brightness;
}

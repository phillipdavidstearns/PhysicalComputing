void getTemp() {
  byte pipeNum = 0; //variable to hold which reading pipe sent data
  while (radio.available(&pipeNum)) { //Check if received data
    float data; //variable to store received value
    radio.read( &data, sizeof(float)); //read value
    if (data >= 50 && data <= 90) { // only accepting readings within 50 - 90 degrees f
      addTemp(data);
      tempLast[pipeNum] = data;
    }
  }
}

float addTemp(float _temp) {
  tempLastTen[tempCounts] = _temp;
  tempCounts++;
  tempCounts %= tempSamples;
  return _temp;
}

float getAvgTemp() {
  float sum = 0;
  float avgs = 0;

  for (int i = 0 ; i < tempSamples ; i ++) {
    sum += tempLastTen[i];
  }
  avgTemps[avgTempCounts] = sum / tempSamples;
  avgTempCounts++;
  avgTempCounts %= avgTempSamples;

  for (int j = 0 ; j < avgTempSamples ; j ++) {
    avgs += avgTemps[j];
  }
  return avgs / avgTempSamples;
}

void updateMinMaxTemp() {
  minTemp = composition[compCount][0] + offsetTemp;
  maxTemp = composition[compCount][1] + offsetTemp;
}


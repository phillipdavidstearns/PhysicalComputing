// Setting up Serial
//Modified from the original example by Example by Tom Igoe


import processing.serial.*;
Serial arduinoPort;

long counter;


/////////////////////////////////SETUP///////////////////////////////

void setup() {
  size(10, 10);
  frameRate(30);
  counter = 0xFF; //used for testLights()
  printArray(Serial.list()); // List all the available serial ports:
  arduinoPort = new Serial(this, Serial.list()[2], 115200); // Open the port you are using at the rate you want:
}

/////////////////////////////////MAIN LOOP///////////////////////////////

void draw() {
  testLights();
}

/////////////////////////////////FUNCTIONS///////////////////////////////



void testLights() {
  byte data = 0;
  arduinoPort.write(0xA0);
  for (int i = 0; i < 4; i++) {
    data = byte((counter >> (i*8)) & 0xFF);
    arduinoPort.write(data);
  }
  println(counter);
  counter = counter << 1 ^ ((counter >> 31) & 1);
}
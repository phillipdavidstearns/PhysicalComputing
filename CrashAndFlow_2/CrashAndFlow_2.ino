/*
   32 Light control system code for Arduino Nano
   Made for Crash and Flow 2.0 a concert by Krussia

   Reads a 4 byte (32bit) "command" from the serial port and outputs it to a
   32 bit serial to parallel array made from daisy chained CD4094 CMOS shift registers

   Note that modifying this code for other Arduino platforms will require changin
   pin numbers and IO/PORT. Here pins 10, 11, 12, are Strobe, Data, and Clock rsp.
   IO is controlled via PORTB on Nano and Uno/Deumillanova boards.

*/

//Timer1 used maintain a steady refresh rate of the lamps.
#include <TimerOne.h>

//display register control pins
#define STROBE 10
#define DATA 11
#define CLK 12

byte lights[4];
volatile unsigned long int count;

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(1e6 / 30); // period in micro seconds (1x10^6 / frames per second)
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(STROBE, OUTPUT);

  Serial.begin(115200);
}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  serial();
}

void callback() {
  updateRegisters();
  //count++;
}

void serial() {
  if (Serial.available() >= 5) {
    if (Serial.read() == 0xA0) { // 0xA0 is an arbitrary sync bit being transmitted by the processing patch before every transmission 
      for (int i = 0 ; i < 4; i++) {
        lights[i] = Serial.read();
      }
    }
  }
}

void updateRegisters() {
  //load data into 4094 shift registers MSB to LSB
  for (int i = 3 ; i >= 0 ; i--) { 
   for(int j = 7 ; j >= 0 ; j--){
    PORTB = ((lights[i] >> j) & 1) << 3;
    //pulse clock
    PORTB |= B00010000;
    PORTB = B00000000;
   }
  }
  //pulse strobe to update 4094 shift registers
  PORTB = B00000100;
  PORTB = B00000000;
}

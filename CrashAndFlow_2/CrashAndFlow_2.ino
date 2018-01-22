//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>

//display register control pins
#define STROBE 10
#define DATA 11
#define CLK 12

volatile unsigned long int count = 0;
volatile unsigned long int lights = 0;

bool link = false;


//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(1e6 / 60); // 16666.67 = 60fps
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(STROBE, OUTPUT);
  bool timeout = 5000;
  int initTime = millis();
  Serial.begin(115200);
}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  serial();
}

void callback() {
  updateRegisters(lights);
  count++;
}

void serial() {
  if (Serial.available() >= 4) {
    lights = 0;
    for (int i = 0 ; i < 4; i++) {
      lights |= Serial.read() << (i * 8);
    }
  }
}

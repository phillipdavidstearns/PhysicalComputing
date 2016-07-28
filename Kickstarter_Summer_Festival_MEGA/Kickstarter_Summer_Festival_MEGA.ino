//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>

//total length of shift register
#define REGISTER_SIZE 16  //

//display register control pins

#define OUTPUT_REG_STROBE 12
#define OUTPUT_REG_DATA 11
#define OUTPUT_REG_CLK 10

volatile unsigned long int REG1 = 1;
volatile unsigned long int REG2 = 0;

int MODE = 2;

//boolean update = true;

long int FREQUENCY = 10000;
long int MULTIPLIER = 8;

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(FREQUENCY * MULTIPLIER); // 16666.67 = 60fps
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  pinMode(OUTPUT_REG_CLK, OUTPUT);
  pinMode(OUTPUT_REG_DATA, OUTPUT);
  pinMode(OUTPUT_REG_STROBE, OUTPUT);

}

//////////////////////////////////////////////////////////////////////////////

void loop() {
}

void displayRegisters() {

  //load data into 4094 shift registers MSB to LSB
  for (int i = REGISTER_SIZE - 1 ; i >= 0 ; i--) {
    PORTB = ((REG1 >> i & 1) ^ ((REG2 >> ((REGISTER_SIZE - 1) - i)) & 1)) << 5;
    //pulse clock
    PORTB |= B00010000;
    PORTB = B00000000;
  }
  //pulse strobe to update 4094 shift registers
  PORTB = B01000000;
  PORTB = B00000000;
}

void callback() {
  switch (MODE) {
    case 0: // shift and circulate
      REG1 = REG1 << 1;
      REG2 = REG2 << 1;
      REG1 |= REG2 >> 16 & 1;
      REG2 |= REG1 >> 16 & 1;
      break;
    case 1: // shift and clear
      REG1 = REG1 << 1;
      REG2 = REG2 << 1;
      break;
    case 2: // random
      REG1 = int(random(pow(2,16)));
      REG2 = int(random(pow(2,16)));
      break;
    case 3: // count
      REG1++;
      REG2++;
      break;
  }

  displayRegisters();

}

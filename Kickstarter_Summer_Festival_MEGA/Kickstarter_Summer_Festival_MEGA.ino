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

int MODE = 7;
unsigned long int RUNTIME = 0;

//boolean update = true;

long int FREQUENCY = 10000;
long int MULTIPLIER = 8;
byte rules = 0;
int bitDepth = 16;

int TIMEOUT = 5000;
int iterations = 0;

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(FREQUENCY * MULTIPLIER); // 16666.67 = 60fps
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  pinMode(OUTPUT_REG_CLK, OUTPUT);
  pinMode(OUTPUT_REG_DATA, OUTPUT);
  pinMode(OUTPUT_REG_STROBE, OUTPUT);

  randomizeRules();

  Serial.begin(9600);
}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  if (iterations >= 128) {
    randomizeRules();
    iterations = 0;
  }
  if ( REG1 == 0 && REG2 == 0) {
    REG1 = int(random(pow(2, 8)));
    REG2 = int(random(pow(2, 8)));
  }
}

void randomizeRules() {
  rules = byte(int(random(pow(2, 8))));
}

int applyRules(int input) {
  int result = 0;
  for (int i = 0 ; i < bitDepth; i++) {
    int state = 0;
    for (int n = 0; n < 3; n++) {
      int coord = ((i + bitDepth + (n - 1)) % bitDepth);
      state |= (input >> coord & 1) << (2 - n);
    }
    result |= ((rules >> state) & 1) << i;
  }
  return result;
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
      REG1 = int(random(pow(2, 16)));
      REG2 = int(random(pow(2, 16)));
      break;
    case 3: // count
      REG1++;
      REG2++;
      break;
    case 4: // shift+recirculate REG1, clear REG2
      REG1 << 1;
      REG2 << 1;
      REG1 |= REG1 >> 16 & 1;
      break;
    case 5: // shift+recirculate REG2, clear REG1
      REG2 << 1;
      REG1 << 1;
      REG2 |= REG2 >> 16 & 1;
      break;
    case 6: // cellular automata
      REG1 = applyRules(REG1);
      REG2 = 0;
      iterations++;
      break;
    case 7:
      REG1 = REG1 << 1;
      REG2 = REG2 << 1;
      REG1 |= REG1 |= REG2 >> 16 & 1 ^ REG2 >> 14 & 1 ^ REG2 >> 13 & 1 ^ REG2 >> 11 & 1;
      REG2 |= REG1 >> 16 & 1;
      break;
  }

  displayRegisters();
}

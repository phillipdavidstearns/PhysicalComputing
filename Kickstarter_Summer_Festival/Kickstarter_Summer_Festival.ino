//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>

//total length of shift register
#define REGISTER_SIZE 32  //

//display register control pins

#define OUTPUT_REG_STROBE 10
#define OUTPUT_REG_DATA 11
#define OUTPUT_REG_CLK 12

volatile unsigned long int REG1 = 1;
volatile unsigned long int REG2 = 0;

int MODE = 7;
unsigned long int RUNTIME = 0;

//boolean update = true;

byte rules = 0;
int bitDepth = 32;

int TIMEOUT = 5000;
int iterations = 0;

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(1e6/60); // 16666.67 = 60fps
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  pinMode(OUTPUT_REG_CLK, OUTPUT);
  pinMode(OUTPUT_REG_DATA, OUTPUT);
  pinMode(OUTPUT_REG_STROBE, OUTPUT);

  Serial.begin(9600);
}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  

  displayRegisters();
}

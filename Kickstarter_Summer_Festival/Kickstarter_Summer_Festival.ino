//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>

//total length of shift register
#define REGISTER_SIZE 16  //

//display register control pins

#define OUTPUT_REG_STROBE 12
#define OUTPUT_REG_DATA 11
#define OUTPUT_REG_CLK 10


volatile unsigned long int count = 0;
volatile unsigned long int REG1 = 1;
volatile unsigned long int REG2 = 0;

//boolean update = true;

int DISPLAY_REFRESH_RATE = 1;
volatile long int MOD1_RATE = 1;
volatile long int MOD2_RATE = 1;
long int FREQUENCY = 16666.67;
long int MULTIPLIER = 8;


int TAP_VALUES[4] = {0, 0, 0, 0}; //stores the tap position values 0-63, 0-31 = Left Register; 32-63 = Right Register;
int TAP_EN[4] = {0, 0, 0, 0}; // uses 0 - 3 to designate which taps are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = bothvoltatile
volatile boolean MOD_STATES[4] = {0, 0, 0, 0};
int MOD_EN[4] = {0, 0, 0, 0}; // uses 0 - 3 to designate which mod inputs are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = both

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
    PORTB = ((REG1 >> i & 1) ^ ((REG2 >> ((REGISTER_SIZE - 1) - i)) & 1)) << 3;
    //pulse clock
    PORTB |= B00000100;
    PORTB = B00000000;
  }
  //pulse strobe to update 4094 shift registers
  PORTB = B00010000; //digitalWrite(OUTPUT_REG_STROBE, 1);
  PORTB = B00000000; //digitalWrite(OUTPUT_REG_STROBE, 0);
}
void callback() {
  REG1 = REG1 << 1;
  REG2 = REG2 << 1;
  
  REG1 |= (REG2 >> 16 & 1) ^ (REG2 >> 13 & 1);
  REG2 |= (REG1 >> 16 & 1) ^ (REG1 >> 11 & 1);

  displayRegisters();

}

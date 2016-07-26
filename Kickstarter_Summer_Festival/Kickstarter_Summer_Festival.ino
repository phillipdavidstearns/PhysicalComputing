//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>

//total length of shift register
#define REGISTER_SIZE 16  //

//display register control pins

#define OUTPUT_REG_STROBE 12
#define OUTPUT_REG_DATA 11
#define OUTPUT_REG_CLK 10

//#define REG_L_OUT 2
//#define REG_R_OUT 3
//#define REG_L_AUX_OUT 4
//#define REG_R_AUX_OUT 5

volatile boolean REG_STATES[REGISTER_SIZE]; //used as two independent registers, REG_L (0-31) and REG_R (32-63)
volatile unsigned long int count = 0;
volatile boolean REG_L_IN;
volatile boolean REG_R_IN;
volatile unsigned long int REG = 1;
//volatile boolean L_DIV_TWO;
//volatile boolean R_DIV_TWO;

//volatile boolean LAST_REG_L_IN;
//volatile boolean LAST_REG_R_IN;

boolean update = true;

int DISPLAY_REFRESH_RATE = 1;
volatile long int MOD1_RATE = 1;
volatile long int MOD2_RATE = 1;
long int FREQUENCY = 8192;
long int MULTIPLIER = 16;

byte BYTE_1 = 0;
byte BYTE_2 = 0;

volatile byte YO = 0;

int TAP_VALUES[4] = {0, 0, 0, 0}; //stores the tap position values 0-63, 0-31 = Left Register; 32-63 = Right Register;
int TAP_EN[4] = {0, 0, 0, 0}; // uses 0 - 3 to designate which taps are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = bothvoltatile
volatile boolean MOD_STATES[4] = {0, 0, 0, 0};
int MOD_EN[4] = {0, 0, 0, 0}; // uses 0 - 3 to designate which mod inputs are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = both

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(50000); //30fps
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  //open serial communication
  Serial.begin(9600);

  //initialize the register state arrays
  for (int i = 0; i < REGISTER_SIZE; i++) {
    if (i == 0) {
      REG_STATES[i] = true;
    } else { 
      REG_STATES[i] = true;
    }
  }

  //  L_DIV_TWO = false;
  //  R_DIV_TWO = false;
  //  LAST_REG_L_IN = false;
  //  LAST_REG_R_IN = false;

//  pinMode(REG_L_OUT, OUTPUT);
//  pinMode(REG_R_OUT, OUTPUT);
//  pinMode(REG_L_AUX_OUT, OUTPUT);
//  pinMode(REG_R_AUX_OUT, OUTPUT);

  pinMode(OUTPUT_REG_CLK, OUTPUT);
  pinMode(OUTPUT_REG_DATA, OUTPUT);
  pinMode(OUTPUT_REG_STROBE, OUTPUT);

}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  //REG++;
}

void displayRegisters() {

  for (int i = 0 ; i < REGISTER_SIZE ; i++) {
    PORTB = ((REG >> i) & 1) << 3;
    //PORTB = (REG_STATES[15 - i] ^ REG_STATES[16 + i]) << 3; //digitalWrite(OUTPUT_REG_DATA, ); 16 light version
    //    PORTB = (REG_STATES[31 - i] ^ REG_STATES[32 + i]) << 3; //digitalWrite(OUTPUT_REG_DATA, ); 32 light version
    PORTB |= B00000100; //digitalWrite(OUTPUT_REG_CLK, 1);
    PORTB = B00000000; //digitalWrite(OUTPUT_REG_CLK, 0);
  }

  PORTB = B00010000; //digitalWrite(OUTPUT_REG_STROBE, 1);
  PORTB = B00000000; //digitalWrite(OUTPUT_REG_STROBE, 0);
}

void callback() {

//  REG_L_IN = 0;
//  REG_R_IN = 0;

//  if (count % MOD1_RATE == 0) {
//    MOD_STATES[0] = !MOD_STATES[0];
//  }
//
//  if (count % MOD2_RATE == 0) {
//    MOD_STATES[1] = !MOD_STATES[1];
//  }

//  for (int i = 0 ; i < 4 ; i ++) {
//    REG_L_IN ^= (REG_STATES[TAP_VALUES[i]] & (TAP_EN[i] & B00000001 )) ^ (MOD_STATES[i] & (MOD_EN[i] & B00000001 ));
//    REG_R_IN ^= (REG_STATES[TAP_VALUES[i]] & ((TAP_EN[i] & B00000010 ) >> 1)) ^ (MOD_STATES[i] & ((MOD_EN[i] & B00000010) >> 1));
//  }


//shift the registers
//  for (int i = REGISTER_SIZE - 1 ; i >= 0 ; i--) {
//    REG_STATES[i] = REG_STATES[i - 1];
//  }

REG = REG << 1;

//update inputs
  REG |= (REG >> 15);
//  REG_STATES[0] = REG_L_IN;
//  REG_STATES[16] = REG_R_IN; // 32 light version
  //  REG_STATES[32] = REG_R_IN; // 64 light version

  //  if(REG_L_IN == 1 && LAST_REG_L_IN == 0) L_DIV_TWO = !L_DIV_TWO;
  //  if(REG_R_IN == 1 && LAST_REG_R_IN == 0) R_DIV_TWO = !R_DIV_TWO;

// PORTD = (REG_L_IN ^ REG_R_IN ^ REG_STATES[TAP_VALUES[0]]) << 2 | (REG_L_IN ^ REG_R_IN ^ REG_STATES[TAP_VALUES[1]]) << 3;


    displayRegisters();
//  if (count % DISPLAY_REFRESH_RATE == 0) {
//    displayRegisters();
//  }

  //  LAST_REG_L_IN = REG_L_IN;
  //  LAST_REG_R_IN = REG_R_IN;



  count++;
}

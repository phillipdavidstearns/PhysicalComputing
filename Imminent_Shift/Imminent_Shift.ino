//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>

//total length of shift register
#define REGISTER_SIZE 32  //  <- for 16 light version
//#define REGISTER_SIZE 64  //  <- for 32 light version

//display register control pins
#define OUTPUT_REG_CLK 10
#define OUTPUT_REG_DATA 11
#define OUTPUT_REG_STROBE 12

#define REG_L_OUT 2
#define REG_R_OUT 3

volatile boolean REG_STATES[REGISTER_SIZE]; //used as two independent registers, REG_L (0-31) and REG_R (32-63)
volatile int count = 0;
volatile boolean REG_L_IN;
volatile boolean REG_R_IN;

boolean update = true;

int DISPLAY_REFRESH_RATE = 1;
long int FREQUENCY = 8192;
long int MULTIPLIER = 16;

byte BYTE_1 = 0;
byte BYTE_2 = 0;

int TAP_VALUES[4] = {15, 31, 0, 0}; //stores the tap position values 0-63, 0-31 = Left Register; 32-63 = Right Register;
int TAP_EN[4] = {1, 2, 0, 0}; // uses 0 - 3 to designate which taps are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = both
boolean MOD_STATES[4] = {0, 0, 0, 0};
int MOD_EN[4] = {0, 0, 0, 0};

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(FREQUENCY * MULTIPLIER);
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  //open serial communication
  Serial.begin(9600);

  //initialize the register state arrays
  for (int i = 0; i < REGISTER_SIZE; i++) {
    if (i == 0) {
      REG_STATES[i] = true;
    } else {
      REG_STATES[i] = false;
    }
  }

  pinMode(REG_L_OUT, OUTPUT);
  pinMode(REG_R_OUT, OUTPUT);
  pinMode(OUTPUT_REG_CLK, OUTPUT);
  pinMode(OUTPUT_REG_DATA, OUTPUT);
  pinMode(OUTPUT_REG_STROBE, OUTPUT);

}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  serial();
}

void serial() {

  if (Serial.available() >= 2) {
    //    Serial.println("Byte RCV'd");
    // get incoming bytes if there are two or more in the queue/buffer:

    BYTE_1 = Serial.read();
    BYTE_2 = Serial.read();

    //BYTE_1 selects target for BYTE_2 data
    //DDDDDDDD DDDDD000=clock, DDDDDDDD DDDDD sets the clock frequency
    //DDDDDDDD XXXXX001=clock frequency multiplier, DDDDDDDD sets the multiplication factor
    //XXPPPPPP XXXDD010=tap position values, DD selects which TAP, PPPPPP sets the position value
    //RLRLRLRL XXXXX011=tap enable values, RL,RL,RL,RL set whether this data is passed to L, R registers neither or both.
    //XXXXDDDD XXXXX100=modulation position values, DDDD directly sets the MOD state,
    //RLRLRLRL XXXXX101=modulation enable values, RL,RL,RL,RL set whether this data is passed to L, R registers neither or both.
    //DDDDDDDD xxxxx000=refresh rate, DDDDDDDD sets the refreh rate (relative to clock frequency), 0 is stop, 1 = realtime, n > 1 = every n register clock cycles
    //
    //BYTE_1 of 11111111 allows direct setting of the register. The next eight bytes are loaded into BYTE_1 where the first byte received is = 0 - 7

    int ADDRESS = BYTE_1 & B00000111;

    if (ADDRESS == 0) {
      //register clock frequency
      if (((BYTE_1 >> 3) | (BYTE_2 << 8)) == 0) {
        Timer1.detachInterrupt();
        update = false;
      } else if (update == false) {
        Timer1.attachInterrupt(callback);
        FREQUENCY = (BYTE_1 >> 3) | (BYTE_2 << 5);
        update = true;
      } else {
        FREQUENCY = (BYTE_1 >> 3) | (BYTE_2 << 5);
      }
    } else if (ADDRESS == 1) {
      //clock multiplier
      MULTIPLIER = (BYTE_1 >> 3) | (BYTE_2 << 5);
    } else if (ADDRESS == 2) {
      //TAP Values
      TAP_VALUES[(BYTE_1 & B00011000) >> 3] = BYTE_2;
    } else if (ADDRESS == 3) {
      //TAP Enables
      TAP_EN[0] = (BYTE_2 & B00000011);
      TAP_EN[1] = (BYTE_2 & B00001100) >> 2;
      TAP_EN[2] = (BYTE_2 & B00110000) >> 4;
      TAP_EN[3] = (BYTE_2 & B11000000) >> 6;
    } else if (ADDRESS == 4) {
      //MOD Values
      MOD_STATES[0] = (BYTE_2 & B00000001);
      MOD_STATES[1] = (BYTE_2 & B00000010) >> 1;
      MOD_STATES[2] = (BYTE_2 & B00000100) >> 2;
      MOD_STATES[3] = (BYTE_2 & B00001000) >> 3;
    } else if (ADDRESS == 5) {
      //MOD Enables
      MOD_EN[0] = (BYTE_2 & B00000011);
      MOD_EN[1] = (BYTE_2 & B00001100) >> 2;
      MOD_EN[2] = (BYTE_2 & B00110000) >> 4;
      MOD_EN[3] = (BYTE_2 & B11000000) >> 6;
    } else if (ADDRESS == 6) {
      DISPLAY_REFRESH_RATE = BYTE_2;
    }
      else if (ADDRESS == 7) {
      REG_STATES[0] = 1;
    }

    if ((FREQUENCY * MULTIPLIER) < 175) {
      Timer1.setPeriod(175);
    } else {
      Timer1.setPeriod(FREQUENCY * MULTIPLIER);
    }

  }
}

void displayRegisters() {

  for (int i = 0 ; i < REGISTER_SIZE / 2 ; i++) {
    PORTB = (REG_STATES[15 - i] ^ REG_STATES[16 + i]) << 3; //digitalWrite(OUTPUT_REG_DATA, ); 32 light version
//    PORTB = (REG_STATES[31 - i] ^ REG_STATES[32 + i]) << 3; //digitalWrite(OUTPUT_REG_DATA, ); 64 light version
    PORTB |= B00000100; //digitalWrite(OUTPUT_REG_CLK, 1);
    PORTB = B00000000; //digitalWrite(OUTPUT_REG_CLK, 0);
  }

  PORTB = B00010000; //digitalWrite(OUTPUT_REG_STROBE, 1);
  PORTB = B00000000; //digitalWrite(OUTPUT_REG_STROBE, 0);

}

void callback() {

  REG_L_IN = 0;
  REG_R_IN = 0;

  for (int i = 0 ; i < 4 ; i ++) {
    REG_L_IN ^= (REG_STATES[TAP_VALUES[i]] & (TAP_EN[i] & B00000001 )) ^ (MOD_STATES[i] & (MOD_EN[i] & B00000001 ));
    REG_R_IN ^= (REG_STATES[TAP_VALUES[i]] & ((TAP_EN[i] & B00000010 ) >> 1)) ^ (MOD_STATES[i] & ((MOD_EN[i] & B00000010) >> 1));
  }

  for (int i = REGISTER_SIZE - 1 ; i >= 0 ; i--) {
    REG_STATES[i] = REG_STATES[i - 1];
  }

  REG_STATES[0] = REG_L_IN;
  REG_STATES[16] = REG_R_IN; // 32 light version
//  REG_STATES[32] = REG_R_IN; // 64 light version

  PORTD = (REG_L_IN << 2) | (REG_R_IN << 3);

  if (count % DISPLAY_REFRESH_RATE == 0) {
    displayRegisters();
  }

  count++;
}

//Timer1 used to handle internal clocking of the registers
#include <TimerOne.h>
#include <musicAtoms.h>

//total length of shift register
#define REGISTER_SIZE 32  //  <- for 16 light version
//#define REGISTER_SIZE 64  //  <- for 32 light version

//display register control pins
#define OUTPUT_REG_CLK 10
#define OUTPUT_REG_DATA 11
#define OUTPUT_REG_STROBE 12
#define REG_L_OUT 2
#define REG_R_OUT 3
#define REG_L_AUX_OUT 4
#define REG_R_AUX_OUT 5

volatile unsigned long int count = 0;


boolean update = true;
int count1 = 0;
int count2 = 0;
int seq1 = 0;
int seq2 = 0;


long int FREQUENCY = 256;
long int MULTIPLIER = 1;

byte BYTE_1 = 0;
byte BYTE_2 = 0;

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(FREQUENCY * MULTIPLIER);
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  //open serial communication
  Serial.begin(9600);

  pinMode(REG_L_OUT, OUTPUT);
  pinMode(REG_R_OUT, OUTPUT);
  pinMode(REG_L_AUX_OUT, OUTPUT);
  pinMode(REG_R_AUX_OUT, OUTPUT);
  pinMode(OUTPUT_REG_CLK, OUTPUT);
  pinMode(OUTPUT_REG_DATA, OUTPUT);
  pinMode(OUTPUT_REG_STROBE, OUTPUT);

}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  //serial();
}

//void serial() {
//
//  if (Serial.available() >= 2) {
//    // Serial.println("Byte RCV'd");
//    // get incoming bytes if there are two or more in the queue/buffer:
//
//    BYTE_1 = Serial.read();
//    BYTE_2 = Serial.read();
//
//    //BYTE_1 selects target for BYTE_2 data
//    //DDDDDDDD DDDDD000=clock, DDDDDDDD DDDDD sets the clock frequency
//    //DDDDDDDD XXXXX001=clock frequency multiplier, DDDDDDDD sets the multiplication factor
//    //XXPPPPPP XXXDD010=tap position values, DD selects which TAP, PPPPPP sets the position value
//    //RLRLRLRL XXXXX011=tap enable values, RL,RL,RL,RL set whether this data is passed to L, R registers neither or both.
//    //XXXXDDDD XXXXX100=modulation position values, DDDD directly sets the MOD state,
//    //RLRLRLRL XXXXX101=modulation enable values, RL,RL,RL,RL set whether this data is passed to L, R registers neither or both.
//    //DDDDDDDD xxxxx110=refresh rate, DDDDDDDD sets the refreh rate (relative to clock frequency), 0 is stop, 1 = realtime, n > 1 = every n register clock cycles
//    //DDDDDDDD DDDDM111= Internal Mod rates where DDDDDDDD DDDD is the mod rate and M is the MOD ID 0 = MOD1_RATE, 1 = MOD2_RATE
//
//    int ADDRESS = BYTE_1 & B00000111;
//
//    if (ADDRESS == 0) {
//      //register clock frequency
//      if (((BYTE_1 >> 3) | (BYTE_2 << 8)) == 0) {
//        Timer1.detachInterrupt();
//        update = false;
//      } else if (update == false) {
//        Timer1.attachInterrupt(callback);
//        FREQUENCY = (BYTE_1 >> 3) | (BYTE_2 << 5);
//        update = true;
//      } else {
//        FREQUENCY = (BYTE_1 >> 3) | (BYTE_2 << 5);
//      }
//    } else if (ADDRESS == 1) {
//      //clock multiplier
//      MULTIPLIER = (BYTE_1 >> 3) | (BYTE_2 << 5);
//    } else if (ADDRESS == 2) {
//      //TAP Values
//      TAP_VALUES[(BYTE_1 & B00011000) >> 3] = BYTE_2;
//    } else if (ADDRESS == 3) {
//      //TAP Enables
//      TAP_EN[0] = (BYTE_2 & B00000011);
//      TAP_EN[1] = (BYTE_2 & B00001100) >> 2;
//      TAP_EN[2] = (BYTE_2 & B00110000) >> 4;
//      TAP_EN[3] = (BYTE_2 & B11000000) >> 6;
//    } else if (ADDRESS == 4) {
//      //MOD Values
//      MOD_STATES[0] = (BYTE_2 & B00000001);
//      MOD_STATES[1] = (BYTE_2 & B00000010) >> 1;
//      MOD_STATES[2] = (BYTE_2 & B00000100) >> 2;
//      MOD_STATES[3] = (BYTE_2 & B00001000) >> 3;
//    } else if (ADDRESS == 5) {
//      //MOD Enables
//      MOD_EN[0] = (BYTE_2 & B00000011);
//      MOD_EN[1] = (BYTE_2 & B00001100) >> 2;
//      MOD_EN[2] = (BYTE_2 & B00110000) >> 4;
//      MOD_EN[3] = (BYTE_2 & B11000000) >> 6;
//    } else if (ADDRESS == 6) {
//      DISPLAY_REFRESH_RATE = BYTE_2;
//    }
//    else if (ADDRESS == 7) {
//      if ((BYTE_1 & B00001000) >> 3 == 0) {
//        MOD1_RATE = (BYTE_1 >> 4) | (BYTE_2 << 4);
//      } else if ((BYTE_1 & B00001000) >> 3 == 1) {
//        MOD2_RATE = (BYTE_1 >> 4) | (BYTE_2 << 4);
//      }
//    }
//
//    if ((FREQUENCY * MULTIPLIER) < 200) {
//      Timer1.setPeriod(200);
//    } else {
//      Timer1.setPeriod(FREQUENCY * MULTIPLIER);
//    }
//
//  }
//}



void callback() {
  
//  if (count % 7 == 1) count1++;
//  if (count1 % 3 == 1) count2++;
  
  count1 = counter1(count >> 0 & 1);
  count2 = counter2(count1 >> 0 & 1);
  
  if (count % 512 == 0) seq1 = int(random(1,16));
  seq1 %= 12;
  if (seq1 == 0) seq1 = 1;
  if (count % seq1 == 0) count1++;

 //PORTD = (count1 >> 0 & 1) << 2 | (count1 % 3 >> 0 & 1) << 3;

  count++;

}

int counter1(boolean _clock){
  static int last_clock;
  static long unsigned int _count1;
  if(_clock == 1 && last_clock == 0) _count1++;
  last_clock=_clock;
  return _count1;
}

int counter2(boolean _clock){
  static int last_clock;
  static long unsigned int _count2;
  if(_clock == 1 && last_clock == 0) _count2++;
  last_clock=_clock;
  return _count2;
}



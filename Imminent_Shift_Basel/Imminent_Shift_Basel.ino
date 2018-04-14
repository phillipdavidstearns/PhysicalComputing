//Timer1 used to handle internal clocking of the registers
//#include <musicAtoms.h>
#include <TimerOne.h>

#define REG_L_OUT 2
#define REG_R_OUT 3

ShiftRegister S1;
DivideByN D1;

volatile uint32_t REG_L = 0;
volatile uint32_t REG_R = 0;

volatile unsigned long int count = 0;
volatile boolean REG_L_IN;
volatile boolean REG_R_IN;


boolean update = true;

int DISPLAY_REFRESH_RATE = 1;
volatile long int MOD1_RATE = 1;
volatile long int MOD2_RATE = 1;
long int FREQUENCY = 8192;
long int MULTIPLIER = 16;

byte BYTE_1 = 0;
byte BYTE_2 = 0;

int TAP_VALUES[4] = {0, 0, 0, 0}; //stores the tap position values 0-63, 0-31 = Left Register; 32-63 = Right Register;
int TAP_EN[4] = {0, 0, 0, 0}; // uses 0 - 3 to designate which taps are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = bothvoltatile
volatile boolean MOD_STATES[4] = {0, 0, 0, 0};
int MOD_EN[4] = {0, 0, 0, 0}; // uses 0 - 3 to designate which mod inputs are enabled for each bit output to the left and right registers 0 or 0x00 = neither, 1 or 0x01 = Left, 2 or 0x10 = Right, 3 or 0x11 = both

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(FREQUENCY * MULTIPLIER);
  Timer1.attachInterrupt(callback);
  
  S1.set(1);
  D1.set(1);
  //note: use setPeriod(period) to adjust the frequency of stuff

  //open serial communication
  Serial.begin(9600);


  pinMode(REG_L_OUT, OUTPUT);
  pinMode(REG_R_OUT, OUTPUT);

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
    //DDDDDDDD xxxxx110=refresh rate, DDDDDDDD sets the refreh rate (relative to clock frequency), 0 is stop, 1 = realtime, n > 1 = every n register clock cycles
    //DDDDDDDD DDDDM111= Internal Mod rates where DDDDDDDD DDDD is the mod rate and M is the MOD ID 0 = MOD1_RATE, 1 = MOD2_RATE

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
      if ((BYTE_1 & B00001000) >> 3 == 0) {
        MOD1_RATE = (BYTE_1 >> 4) | (BYTE_2 << 4);
      } else if ((BYTE_1 & B00001000) >> 3 == 1) {
        MOD2_RATE = (BYTE_1 >> 4) | (BYTE_2 << 4);
      }
    }

    if ((FREQUENCY * MULTIPLIER) < 200) {
      Timer1.setPeriod(200);
    } else {
      Timer1.setPeriod(FREQUENCY * MULTIPLIER);
    }

  }
  D1.set(TAP_VALUES[3]);
}

void callback() {

  REG_L_IN = 0;
  REG_R_IN = 0;

  if (count % MOD1_RATE == 0) {
    MOD_STATES[0] = !MOD_STATES[0];
  }

  if (count % MOD2_RATE == 0) {
    MOD_STATES[1] = !MOD_STATES[1];
  }
  
  for (int i = 0 ; i < 4 ; i ++) {
    REG_L_IN ^= ((REG_L >> TAP_VALUES[i] & 1) & (TAP_EN[i] & B00000001)) ^ (MOD_STATES[i] & (MOD_EN[i] & B00000001 ));
    REG_R_IN ^= ((REG_R >> TAP_VALUES[i] & 1) & ((TAP_EN[i] & B00000010) >> 1)) ^ (MOD_STATES[i] & ((MOD_EN[i] & B00000010) >> 1));
  }

REG_L = REG_L << 1 | REG_L_IN;
REG_R = REG_R << 1 | REG_R_IN;


uint32_t temp = S1.shiftL();
S1.set(temp | TAP_EN[0] & (temp >> TAP_VALUES[0] & 1) ^ TAP_EN[1] & (temp >> TAP_VALUES[1] & 1) ^ (MOD_STATES[0] & MOD_EN[0]));
  
PORTD = D1.run(S1.get(0)) << 2 ;

//PORTD = (REG_L >> TAP_VALUES[0] & 1) << 2 | (REG_R >> TAP_VALUES[1] & 1) << 3;
//PORTD = (REG_L_IN ^ REG_R_IN ^ (REG_L >> TAP_VALUES[0] & 1)) << 2 | (REG_L_IN ^ REG_R_IN ^ (REG_R >> TAP_VALUES[1] & 1)) << 3;
 
 

  count++;
}

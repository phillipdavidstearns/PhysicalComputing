//Timer1 used to handle internal clocking of the registers
#include <musicAtoms.h>
#include <TimerOne.h>

#define REG_L_OUT 2
#define REG_R_OUT 3

volatile uint32_t S1, S2;

uint8_t S1T1 = 1;
bool S1T1E = 1;
uint8_t S1T2 = 0;
bool S1T2E = 0;
bool S1M = 1;

uint8_t S2T1 = 1;
bool S2T1E = 1;
uint8_t S2T2 = 0;
bool S2T2E = 0;
bool S2M = 1;

bool MUTE = 0;

uint8_t C1Q,C2Q;

Counter C1,C2;
DivideByN D1, D2;

uint8_t D1N;
uint8_t D2N;


byte BYTE_1 = 0;
byte BYTE_2 = 0;

//////////////////////////////////////////////////////////////////////////////

void setup() {

  Timer1.initialize(256);
  Timer1.attachInterrupt(callback);

  //note: use setPeriod(period) to adjust the frequency of stuff

  //open serial communication
  Serial.begin(115200);


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
 
/*   

BYTE_1 Addresses

00 - 0000 0000 S1T1
01 - 0000 0001 S1T1E
02 - 0000 0010 S1T2
03 - 0000 0011 S1T2E
04 - 0000 0100 S1M

05 - 0000 0101 S2T1
06 - 0000 0110 S2T1E
07 - 0000 0111 S2T2
08 - 0000 1000 S2T2E
09 - 0000 1001 S2M

10 - 0000 1010 D1_N
11 - 0000 1011 D2_N

10 - 0000 1010 C1Q
11 - 0000 1011 C1Q

15 - 0000 1111

*/

switch(BYTE_1){
  case 0:
    S1T1 = BYTE_2 & B11111;
    break;
  case 1:
    S1T1E = BYTE_2 & 1;
    break;
  case 2:
    S1T2 = BYTE_2 & B11111;
    break;
  case 3:
    S1T2E = BYTE_2 & 1;
    break;
  case 4:
    S1M = BYTE_2 & 1;
    break;
  case 5:
    S2T1 = BYTE_2 & B11111;
    break;
  case 6:
    S2T1E = BYTE_2 & 1;
    break;
  case 7:
    S2T2 = BYTE_2 & B11111;
    break;
  case 8:
    S2T2E = BYTE_2 & 1;
    break;
  case 9:
    S2M = BYTE_2 & 1;
    break;
  case 10:
    D1N = BYTE_2;
    break;
  case 11:
    D2N = BYTE_2;
    break;
  case 12:
    C1Q = BYTE_2;
    break;
  case 13:
    C2Q = BYTE_2;
    break;
  case 15:
    MUTE = BYTE_2 & 1;
    break;
}

  }
}





void callback() {
  
  S1 = S1 << 1 | (((S1 >> S1T1 & 1) & S1T1E) ^ ((S1 >> S1T2 & 1) & S1T2E) ^ S1M);
  S2 = S2 << 1 | (((S2 >> S2T1 & 1) & S2T1E) ^ ((S2 >> S2T2 & 1) & S2T2E) ^ S2M);
  if(MUTE){
    PORTD = (((C1.inc(S2 >> 7 & 1) >> C1Q) & 1) ^ D1.run(S1 & 1, D1N)) << 2 | (((C2.inc(S1 >> 7 & 1) >> C2Q) & 1) ^ D2.run(S2 & 1, D2N)) << 3;
  }
}

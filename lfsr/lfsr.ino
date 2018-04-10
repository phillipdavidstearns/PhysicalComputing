// include the nifty TimerHelpers written by Nick Gammon
#include "TimerHelpers.h"

#define OUT1 8
#define OUT2 9
#define OUT3 10
#define INTERRUPT 2

//master clock
volatile unsigned long mc = 0;

// used for setting parameters vis serial
byte addr = 0;
byte val = 0;

// basic registers
volatile long r1 = 1;
volatile long r2 = 1;
volatile long r3 = 1;

//register taps
byte r1t1 = 5;
byte r1t2 = 0;
byte r2t1 = 6;
byte r2t2 = 0;
byte r3t1 = 7;
byte r3t2 = 0;

//modulation of the lfsr
byte m1 = 0;
byte m2 = 0;
byte m3 = 0;

//index of master clock output to feed into counters
byte q1 = 0;
byte q2 = 0;
byte q3 = 0;

//counters to drive lfsr
int c1 = 0;
int c2 = 0;
int c3 = 0;

//state tracking for triggering counters off the master clock
bool tc1 = 0;
bool tc2 = 0;
bool tc3 = 0;

bool lc1 = 0;
bool lc2 = 0;
bool lc3 = 0;

//divide by n used to trigger lfsr
byte d1 = 0;
byte d2 = 0;
byte d3 = 0;

//output counters for sub octaves
byte co1 = 0;
byte co2 = 0;
byte co3 = 0;

//state tracking for triggering output counters
bool tco1 = 0;
bool tco2 = 0;
bool tco3 = 0;

bool lco1 = 0;
bool lco2 = 0;
bool lco3 = 0;

//select which counter bit to output
byte o1 = 0;
byte o2 = 0;
byte o3 = 0;

//////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(INTERRUPT, INPUT);
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);

  Timer1::setMode (15, Timer1::PRESCALE_1, 0);
  OCR1A = 512;  // 16666us / 4 uS = 4166.5 (less one)
  OCR1B = 0;    // 75us / 4 uS = 18.75 (less one)
  TIFR1 = _BV (TOV1);   // clear overflow flag
  TIMSK1 = _BV (TOIE1);  // interrupt on overflow on timer 1

  //open serial communication
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT), pinISR, RISING);
}

//////////////////////////////////////////////////////////////////////////////

void loop() {
  serial();
}


void serial() {
  if (Serial.available() == 2) {
    //    Serial.println("Byte RCV'd");
    // get incoming bytes if there are two or more in the queue/buffer:
    addr = Serial.read();
    val = Serial.read() & 0x1F; //automatically masks to 5-bit, 0-31 range

    //mapping of serial commands to variables
    //addr:                               val:
    //0  - r1t1 register 1 tap 1          0-31
    //1  - r1t2 register 1 tap 2          0-31
    //2  - r2t1 register 2 tap 1          0-31
    //3  - r2t2 register 2 tap 2          0-31
    //4  - r3t1 register 3 tap 1          0-31
    //5  - r3t2 register 3 tap 2          0-31
    //6  - m1   mod 1                     0-31
    //7  - m2   mod 2                     0-31
    //8  - m3   mod 3                     0-31
    //9  - q1   clock output select       0-31, 0 disables output
    //10 - q2   clock output select       0-31, 0 disables output
    //11 - q3   clock output select       0-31, 0 disables output
    //12 - d1                             0-31, 0 disables output
    //13 - d2                             0-31, 0 disables output
    //14 - d3                             0-31, 0 disables output


    switch (addr) {
      case 0:
        r1t1 = val;
        break;
      case 1:
        r1t2 = val;
        break;
      case 2:
        r2t1 = val;
        break;
      case 3:
        r2t2 = val;
        break;
      case 4:
        r3t1 = val;
        break;
      case 5:
        r3t2 = val;
        break;
      case 6:
        m1 = val;
        break;
      case 7:
        m2 = val;
        break;
      case 8:
        m3 = val;
        break;
      case 9:
        q1 = val;
        break;
      case 10:
        q2 = val;
        break;
      case 11:
        q3 = val;
        break;
      case 12:
        d1 = val;
        break;
      case 13:
        d2 = val;
        break;
      case 14:
        d3 = val;
        break;
      case 15:
        o1 = val;
        break;
      case 16:
        o2 = val;
        break;
      case 17:
        o3 = val;
        break;
      default:
        break;
    }
  }
}

ISR (TIMER1_OVF_vect) {

  //core voices
  tc1 = mc >> q1 & 1; //shift and mask the masterclock to get current clock state
  if (tc1 && !lc1) c1++; //rising edge triggers advance of counter
  lc1 = tc1; //current state becomes last state
  if (c1 % d1 == 0) r1 = lfsr(r1, r1t1, r1t2, m1); //divide by n (d1) and advance the LFSR
  tco1 = r1 & 1; //mask the LSB of the register and set current state
  if (tco1 && !lco1) co1++; //rising edge trigger advance of counter
  lco1 = tco1; //current state becomes last state

  tc2 = mc >> q2 & 1;
  if (tc2 && !lc2) c2++;
  lc2 = tc2;
  if (c2 % d2 == 0) r2 = lfsr(r2, r2t1, r2t2, m2);
  tco2 = r2 & 1;
  if (tco2 && !lco2) co2++;
  lco2 = tco2;

  tc3 = mc >> q3 & 1;
  if (tc3 && !lc3) c3++;
  lc3 = tc3;
  if (c3 % d3 == 0) r3 = lfsr(r3, r3t1, r3t2, m3);
  tco3 = r3 & 1;
  if (tco3 && !lco3) co3++;
  lco3 = tco3;

  PORTB = (co1 >> o1 & 1) | (co2 >> o2 & 1) | (co3 >> o3 & 1) << 1;
  mc++;
}

void pinISR() {
  r1 = 1;
  r2 = 1;
  r3 = 1;
}

long lfsr(long r, byte t1, byte t2, byte m) {
  if (t1 == 0) return r;
  else if (t2 == 0) return (((r >> t1) ^ m) & 1) | (r << 1);
  else return (((r >> t1) ^ (r >> t2) ^ m) & 1) | (r << 1);
}


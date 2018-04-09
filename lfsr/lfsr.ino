// include the nifty TimerHelpers written by Nick Gammon
#include "TimerHelpers.h"

#define OUT1 8
#define OUT2 9
#define OUT3 10
#define INTERRUPT 2

byte addr;
byte val;

volatile long r1 = 1;
volatile long r2 = 1;
volatile long r3 = 1;

byte d1;
byte d2;
byte d3;

byte m1;
byte m2;
byte m3;

byte md1;
byte md2;
byte md3;

byte r1t1 = 13;
byte r1t2 = 14;
byte r2t1 = 15;
byte r2t2 = 16;
byte r3t1 = 0;
byte r3t2 = 0;

volatile unsigned long clock = 0;

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

  attachInterrupt(digitalPinToInterrupt(2), pinISR, RISING);
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
    //9  - md1   clock division modulo 1  0-31, 0 disables output
    //10 - md2   clock division modulo 1  0-31, 0 disables output
    //11 - md3   clock division modulo 1  0-31, 0 disables output
    //12 - d1                             0-31, 0 disables output
    //13 - d2                             0-31, 0 disables output
    //14 - d3                             0-31, 0 disables output


    switch (addr) {
      case 0:
      r1t1=val;
        break;
      case 1:
      r1t2=val;
        break;
      case 2:
      r2t1=val;
        break;
      case 3:
      r2t2=val;
        break;
      case 4:
      r3t1=val;
        break;
      case 5:
      r3t2=val;
        break;
      case 6:
      m1=val;
        break;
      case 7:
      m2=val;
        break;
      case 8:
      m3=val;
        break;
      case 9:
      md1=val;
        break;
      case 10:
      md2=val;
        break;
      case 11:
      md3=val;
        break;
      case 12:
      d1=val;
        break;
      case 13:
      d2=val;
        break;
      case 14:
      d3=val;
        break;
      default:
        break;
    }
  }
}

ISR (TIMER1_OVF_vect) {
  r1 = (r1 >> r1t1  ^ r1 >> r1t2) & 1 | r1 << 1;
  r2 = (r2 >> r2t1  ^ r2 >> r2t2) & 1 | r2 << 1;

  PORTB = (r1 & 1) | (r2 & 1) << 1;
  clock++;
}

void pinISR() {
  r1 = 1;
  r2 = 1;
  r3 = 1;
}


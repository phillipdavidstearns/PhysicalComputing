//Timer1 used to handle internal clocking of the registers
//#include <TimerOne.h>
#include "TimerHelpers.h"
#include <avr/pgmspace.h>
#include <avr/sleep.h>

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

byte r1t1=13;
byte r1t2=14;
byte r2t1=15;
byte r2t2=16;
byte r3t1=0;
byte r3t2=0;

volatile unsigned long clock = 0;

//////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(INTERRUPT, INPUT);
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
//note: use setPeriod(period) to adjust the frequency of stuff
//  Timer1.initialize(128);
//  Timer1.attachInterrupt(timerISR);

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
  //sleep_mode();
}


void serial() {
  if (Serial.available() == 2) {
    //    Serial.println("Byte RCV'd");
    // get incoming bytes if there are two or more in the queue/buffer:
    addr = Serial.read();
    val = Serial.read();
    
    switch(addr){
      case 0:
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

void pinISR(){
  r1t1 = random(32);
  r1t2 = random(32);
  r2t1 = random(32);
  r2t2 = random(32);
}


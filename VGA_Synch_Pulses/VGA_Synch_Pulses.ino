/*
  VGA Synch Pulse Generator

  Modified by: Phillip David Stearns
  Date: 29 March, 2016

  Original Author:   Nick Gammon
  Date:     20th April 2012
  Version:  1.2

  Version 1.0: initial release
  Version 1.1: code cleanups
  Version 1.2: more cleanups, added clear screen (0x0C), added scrolling


  Connections:

  D3 : Horizontal Sync (68 ohms in series) --> Pin 13 on DB15 socket
  D10 : Vertical Sync (68 ohms in series) --> Pin 14 on DB15 socket

  Gnd : --> Pins 5, 6, 7, 8, 10 on DB15 socket

  PERMISSION TO DISTRIBUTE

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
  and associated documentation files (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.


  LIMITATION OF LIABILITY

  The software is provided "as is", without warranty of any kind, express or implied,
  including but not limited to the warranties of merchantability, fitness for a particular
  purpose and noninfringement. In no event shall the authors or copyright holders be liable
  for any claim, damages or other liability, whether in an action of contract,
  tort or otherwise, arising from, out of or in connection with the software
  or the use or other dealings in the software.
*/

#include "TimerHelpers.h"
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#define BETA_ARDUINO ARDUINO < 100

const int hSyncPin = 3;     // <------- HSYNC
const int vSyncPin = 10;    // <------- VSYNC

volatile long count;
volatile long reg1, reg2, reg3;
byte mod1, mod2, mod3;
volatile byte r1t1, r1t2, r2t1, r2t2, r3t1, r3t2, div1, div2, div3;
byte portDOut;
volatile byte addr, value;
// Timer 1 - Vertical sync

// output    OC1B   pin 16  (D10) <------- VSYNC

//   Period: 16.67 mS (60 Hz)
//      1/60 * 1e6 = 16666.66 uS
//    Pulse for 75 uS
//    Sync pulse: 2 lines
//    Back porch: 41 lines
//    Active video: 1080 lines
//    Front porch: 4 lines
//       Total: 1125 lines

// Timer 2 - Horizontal sync

// output    OC2B   pin 5  (D3)   <------- HSYNC

//   Period: 14.81 uS (67.5 kHz)
//      (1/60) / 1125 * 1e6 = 14.81 uS
//   Pulse for .3 uS
//    Sync pulse: 89 pixels
//    Back porch: 719 pixels
//    Active video: 1920 pixels
//    Front porch: 1 pixels
//       Total: 2640 pixels

// Pixel time =  ((1/30) / 1125 * 1e9) / 2640 = 11.22  nS
//  frequency =  1 / 11.22  nS = 89.1 MHz

void setup()
{
  reg1 = 1;
  reg2 = 1;
  reg3 = 1;

  mod1 = 7;
  mod2 = 9;
  mod3 = 9;

  r1t1 = 5;
  r1t2 = 0;
  r2t1 = 6;
  r2t2 = 0;
  r3t1 = 7;
  r3t2 = 0;

  div1 = 7;
  div2 = 5;
  div3 = 6;

  portDOut = 0;

  pinMode (5, OUTPUT);
  pinMode (6, OUTPUT);
  pinMode (7, OUTPUT);

  addr = 0;
  value = 0;

  // disable Timer 0
  TIMSK0 = 0;  // no interrupts on Timer 0
  OCR0A = 0;   // and turn it off
  OCR0B = 0;

  // Timer 1 - vertical sync pulses
  pinMode (vSyncPin, OUTPUT);
  Timer1::setMode (15, Timer1::PRESCALE_64, Timer1::CLEAR_B_ON_COMPARE);
  OCR1A = 4165;  // 16666us / 4 uS = 4166.5 (less one)
  OCR1B = 18;    // 75us / 4 uS = 18.75 (less one)
  TIFR1 = _BV (TOV1);   // clear overflow flag
  TIMSK1 = _BV (TOIE1);  // interrupt on overflow on timer 1

  // Timer 2 - horizontal sync pulses
  pinMode (hSyncPin, OUTPUT);
  Timer2::setMode (7, Timer2::PRESCALE_1, Timer2::CLEAR_B_ON_COMPARE);
  OCR2A = 236;   // 14.81us / 0.0625 uS =  237 (less one)
  OCR2B = 4;    // .3us / 0.0625 uS = 5 (less one)
  TIFR2 = _BV (TOV2);   // clear overflow flag
  TIMSK2 = _BV (TOIE2);  // interrupt on overflow on timer 2

  //Serial.begin(115200);

  // prepare to sleep between horizontal sync pulses
  set_sleep_mode (SLEEP_MODE_IDLE);
}  // end of setup

// ISR: Vsync pulse
ISR (TIMER1_OVF_vect){

} // end of TIMER1_OVF_vect

// ISR: Hsync pulse ... this interrupt merely wakes us up
ISR (TIMER2_OVF_vect)
{
//  if (count % div1 == 0) reg1 = lfsr(reg1, r1t1 , r1t2, (count >> mod1) & 1);
//  if (count % div2 == 0) reg2 = lfsr(reg2, r2t1 , r2t2, (count >> mod2) & 1);
//  if (count % div3 == 0) reg3 = lfsr(reg3, r3t1 , r3t2, (count >> mod3) & 1);
//  PORTD = B00000011;
//  PORTD |= (((reg1 & 1) << 7) | ((reg2 & 1) << 6) | ((reg3 & 1) << 5));
//  count++;
} // end of TIMER2_OVF_vect

void loop()
{
  
//  if (Serial.available() >= 2) {
//    Serial.println("Byte RCV'd");
//    addr = Serial.read();
//    value = Serial.read();
//
//    switch (addr) {
//      case 0: // reg1 tap1
//        r1t1 = value;
//        break;
//      case 1:// reg1 tap2
//        r1t2 = value;
//        break;
//      case 2:// reg2 tap1
//        r2t1 = value;
//        break;
//      case 3:// reg2 tap2
//        r2t2 = value;
//        break;
//      case 4:// reg3 tap1
//        r3t1 = value;
//        break;
//      case 5:// reg3 tap2
//        r3t2 = value;
//        break;
//      case 6: // mod1
//        mod1 = value;
//        break;
//      case 7: //mod2
//        mod2 = value;
//        break;
//      case 8: //mod3
//        mod3 = value;
//        break;
//      case 9: //div1
//        div1 = value;
//        break;
//      case 10: //div2
//        div2 = value;
//        break;
//      case 11: //div3
//        div3 = value;
//        break;
//    }
//  }

  // sleep to ensure we start up in a predictable way
  sleep_mode ();
}  // end of loop

long lfsr(long reg, byte tap1, byte tap2, boolean mod) {
  if (tap2 != 0) return ((reg >> tap1 ^ reg >> tap2 ^ mod) & 1) | (reg << 1);
  else return ((reg >> tap1 ^ mod) & 1) | (reg << 1);
}


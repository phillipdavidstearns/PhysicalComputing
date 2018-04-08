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
ISR (TIMER1_OVF) {
} // end of TIMER1_OVF

// ISR: Hsync pulse ... this interrupt merely wakes us up
ISR (TIMER2_OVF) {
} // end of TIMER2_OVF

void loop() {
  // sleep to ensure we start up in a predictable way
  sleep_mode();
}  // end of loop


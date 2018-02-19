// Dimmer Node Software
// Code used to control the lighting for 
// New Max by Madeline Hollander 2018

// An nRF24L01 receives a 1-byte command which drives a Triac based AC dimmer circuit (PowerSwitchTail PSSR/ZC Tail)

// nRF24L01 utilization draws from this tutorial http://www.instructables.com/id/NRF24L01-Multiceiver-Network/

// Dimming based on code from Step 6: Arduino Controlled Light Dimmer: the Software III
// http://www.instructables.com/id/Arduino-controlled-light-dimmer-The-circuit/

// Regarding the timing for the phase modulation dimming of the triac:
// For 60 Hz mains power and 256 steps (full 8bit resolution) the time delay perstep is 32.5 microseconds
// There are two zero crossing per cycle, so their frequency is double that of the mains.
// (120 Hz=8333uS) / 256 brightness steps = 33 uS / brightness step

#include <TimerOne.h>         // Avaiable from http://www.arduino.cc/playground/Code/Timer1
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

#define CHAN 108
const int pinCE = 3; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 4; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
RF24 radio(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection

// nRF24l01 pinout (top down)
// [0] 1
//  2  3
//  4  5
//  6  7

//  Wiring  nRF24   Arduino Uno/Nano Pins
//  GND     1       GND
//  3.3V    2       3.3V (Vout from AMS1117-3.3 DC Voltage Regulator connected to Arduino Vin and Ground)       
//  CE      3       2 (see code above)
//  CSN     4       3 (see code above)
//  SCK     5       13
//  MOSI    6       11
//  MISO    7       12
//  IRQ     8       NOT CONNECTED

/*************/
#define node 6
// 1 - 4 = Temperature Nodes
// 5 = ACNode
// 6 = Dimmer Node
/*************/

const uint64_t wAddress[] = {
  0xB3B4B5B6AALL,
  0xB3B4B5B6BBLL,
  0xB3B4B5B6CCLL,
  0xB3B4B5B6DDLL,
  0xB3B4B5B6EELL,
  0xB3B4B5B6FFLL
};

const uint64_t PTXpipe = wAddress[node - 1]; // Pulls the address from the above array for this node's pipe

//Variables
byte currentDim = 0;             // Dimming level 0 - 255, 0 is brightest, 255 is off
byte targetDim = 0;
int freqStep = 33;               // This is the delay-per-brightness step in microseconds.
volatile int count = 0;          // Variable to use as a counter volatile as it is in an interrupt
volatile boolean zero_cross = 0; // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 7;                  // Output to Opto Triac
int inc = 1;                     // counting up or down, 1=up, -1=down
unsigned long timer = 0;
int fadeTime = 50;              // the time in milliseconds between fade update.
boolean startupFinished = false;

void setup() {
  pinMode(AC_pin, OUTPUT);                         // Set the Triac pin as output
  pinMode(2, INPUT_PULLUP);                        // ZC on pin 2, IRQ 0
  attachInterrupt(0, zero_cross_detect, RISING);   // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                     // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep);     // interrupt to check whether to fire TRIAC, runs every freqStep in microseconds.
  radio.begin();                                   //Start the nRF24 module
  radio.setPALevel(RF24_PA_HIGH);                  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(CHAN);                          // the higher channels tend to be more "open"
  radio.openReadingPipe(0, PTXpipe);               //open reading or receive pipe
  radio.startListening();                          //go into transmit mode
}

void loop()  {
  getData();
  if (millis() - timer > fadeTime) updateDimmer();
}

void updateDimmer() {
  timer = millis();
  if (currentDim > targetDim) {
    currentDim--;
  } else if (currentDim < targetDim) {
    currentDim++;
  }
}

void zero_cross_detect() {
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  count = 0;
}


// Turn on the TRIAC at the appropriate time
void dim_check() {
  if (zero_cross) {
    if (count >= currentDim) {
      digitalWrite(AC_pin, HIGH); // turn on light
      zero_cross = false; //reset zero cross detection
      count = 0; // reset time step counter
    }
    else {
      count++; // increment time step counter
    }
  }
}

void getData() {

  byte pipeNum = 0; //variable to hold which reading pipe sent data
  byte gotByte = 0; //used to store payload from transmit module

  while (radio.available(&pipeNum)) { //Check if received data
    radio.read( &gotByte, 1 ); //read one byte of data and store it in gotByte variable
    if (gotByte == 255) { // if 255, it will set brightness to maxumum immediately.
      currentDim = 0;
      targetDim = 0;
    } else if (gotByte == 0 ) { //if 0, it will turn off the lights immediately.
      currentDim = 255;
      targetDim = 255;
    } else {
      targetDim = 255 - gotByte;
    }
  }
}


// AC Node Software
// Code used to control the air conditioning for 
// New Max by Madeline Hollander 2018

// An nRF24L01 receives a 1-byte command which is used to drive a bank of Solid State Relays switching power to air conditioners

#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/


#define CHAN 108
const int pinCE = 2; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 3; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
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
#define node 5
// 1 - 4 = Temperature Nodes
// 5 = ACNode
// 6 = Dimmer Node
/*************/

#define AC_PIN 7

// RF24 library doesn't enforce, but asks that all but the two least significant bits share the same value
const uint64_t wAddress[] = {
  0xB3B4B5B6AALL,
  0xB3B4B5B6BBLL,
  0xB3B4B5B6CCLL,
  0xB3B4B5B6DDLL,
  0xB3B4B5B6EELL,
  0xB3B4B5B6FFLL
  //--------^^ least two significant bits are unique, all the rest are the same
};

const uint64_t PTXpipe = wAddress[node - 1]; // Pulls the address from the above array for this node's pipe

void setup() {
  pinMode(AC_PIN, OUTPUT);
  radio.begin();            //Start the nRF24 module
  radio.setPALevel(RF24_PA_LOW);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(CHAN);          // the higher channels tend to be more "open"
  radio.openReadingPipe(0, PTXpipe); //open reading or receive pipe
  radio.startListening();
}

void loop()  {
  getData();
}

void getData() {

  byte pipeNum = 0; //variable to hold which reading pipe sent data
  byte gotByte = 0; //used to store payload from transmit module

  while (radio.available(&pipeNum)) { //Check if received data

    radio.read( &gotByte, 1 ); //read one byte of data and store it in gotByte variable

    if (gotByte == 0) {
      digitalWrite(AC_PIN, 0);
    } else if (gotByte == 1 ) {
      digitalWrite(AC_PIN, 1);
    } else {
      digitalWrite(AC_PIN, 0);
    }
    
  }
  
}

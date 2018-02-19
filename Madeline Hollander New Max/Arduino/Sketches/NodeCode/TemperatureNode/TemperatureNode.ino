// Temperature Node Software
// Code used to measure and transmit temperature readings for
// New Max by Madeline Hollander 2018

// An nRF24L01 transmits a temperature value read from a DHT22 sensor.

// nRF24L01 utilization draws from this tutorial http://www.instructables.com/id/NRF24L01-Multiceiver-Network/
// DHT22 utilization draws from this tutorial http://www.ardumotive.com/how-to-use-dht-22-sensor-en.html

#include <DHT.h> //library to interface with the DHT22 temperature and humidity sensor
#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

//DHT22 Definitions
#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

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
#define node 2
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
float temp; //Stores temperature value
unsigned long timer = 0; // used to trigger readings at interval specified by trigger in milliseconds
unsigned int trigger= 1000; 

void setup() {
  dht.begin();
  radio.begin();            //Start the nRF24 module
  radio.setPALevel(RF24_PA_HIGH);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(CHAN);          // the higher channels tend to be more "open"
  radio.openReadingPipe(0, PTXpipe); //open reading or receive pipe
  radio.startListening(); //go into transmit mode
}

void loop()  {
  if(millis() - timer >= trigger){
    timer = millis();
    if (getReading()) sendTemp();
  }
}

bool sendTemp() {
  bool sent; //variable to track if write was successful
  radio.stopListening(); //Stop listening, start receiving data.
  radio.openWritingPipe(PTXpipe); //Open writing pipe to the nRF24 that guessed the right number
  // note that this is the same pipe address that was just used for receiving
  sent = radio.write(&temp, sizeof(float));
  radio.startListening(); //Switch back to a receiver
  return sent;  //return whether write was successful
}

boolean getReading() {
  temp = dht.readTemperature(true); //false by default = Celsius, true = Fahrenheit
  return !(isnan(temp)); // Check if any reads failed
} //end getReading

// An example demonstrating the multiceiver capability of the NRF24L01+
// in a star network with one PRX hub and up to six PTX nodes

//This sketch is a modification from a video on the ForceTronics YouTube Channel,

//which code was leveraged from http://maniacbug.github.io/RF24/starping_8pde-example.html

//This sketch is free to the public to use and modify at your own risk


#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

const int pinCE = 2; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 3; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
RF24 radio(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection
/*************/
#define node 1     // must be a number from 1 - 6 identifying the PTX node
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

bool done = false; //used to know when to stop sending packets
byte data = 0;
void setup()

{
  Serial.begin(115200);   //start serial to communicate process
  radio.begin();            //Start the nRF24 module
  radio.setPALevel(RF24_PA_LOW);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(108);          // the higher channels tend to be more "open"
  radio.openReadingPipe(0, PTXpipe); //open reading or receive pipe
  radio.startListening(); //go into transmit mode
}

void loop()  {
  byte pipeNum = 0; //variable to hold which reading pipe sent data
  byte gotByte = 0; //used to store payload from transmit module

  while (radio.available(&pipeNum)) { //Check if received data
    radio.read( &gotByte, 1 ); //read one byte of data and store it in gotByte variable
    Serial.print("Received data from transmitter: ");
    Serial.println(pipeNum + 1); //print which pipe or transmitter this is from
    Serial.print("They sent number: ");
    Serial.println(gotByte); //print payload or the number the transmitter guessed
    if (gotByte != 0xA0) { //if true they guessed wrong
      Serial.println("Wrong data.");
    }
    else { //if this is true they guessed right
      data = gotByte;
      if (sendEcho(pipeNum)) Serial.println("Handshake successful."); //if true we successfully responded
      else Serial.println("Write failed"); //if true we failed responding
    }
    Serial.println();
  }


  radio.stopListening();
  byte data = 0xA0; //generate random guess between 0 and 10
  radio.openWritingPipe(PTXpipe);        //open writing or transmit pipe
  if (!radio.write( &data, 1 )) { //if the write fails let the user know over serial monitor
    Serial.println("Guess delivery failed");
  }
  else { //if the write was successful
    Serial.print("Success sending guess: ");
    Serial.println(data);
    radio.startListening(); //switch to receive mode to see if the guess was right
    unsigned long startTimer = millis(); //start timer, we will wait 200ms
    bool timeout = false;

    while ( !radio.available() && !timeout ) { //run while no receive data and not timed out
      if (millis() - startTimer > 200 ) timeout = true; //timed out
    }
    if (timeout) Serial.println("Last guess was wrong, try again"); //no data to receive guess must have been wrong
    else  { //we received something so guess must have been right
      byte daNumber; //variable to store received value
      radio.read( &daNumber, 1); //read value
      if (daNumber == data) { //make sure it equals value we just sent, if so we are done
        Serial.println("You guessed right so you are done");
        done = true; //signal to loop that we are done guessing
      }
      else Serial.println("Something went wrong, keep guessing"); //this should never be true, but just in case
    }
    radio.stopListening(); //go back to transmit mode
  }

  delay(1000);
}

bool sendEcho(byte xMitter) {
  bool worked; //variable to track if write was successful
  radio.stopListening(); //Stop listening, start receiving data.
  radio.openWritingPipe(wAddress[xMitter]); //Open writing pipe to the nRF24 that guessed the right number
  // note that this is the same pipe address that was just used for receiving
  if (!radio.write(&data, 1))  worked = false; //write the correct number to the nRF24 module, and check that it was received
  else worked = true; //it was received
  radio.startListening(); //Switch back to a receiver
  return worked;  //return whether write was successful
}

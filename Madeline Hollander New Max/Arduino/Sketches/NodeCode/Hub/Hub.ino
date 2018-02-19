// Code for Main Hub
//
// New Max Degrees by Madeline Hollander
//
// Utilizes the multiceiver capability of the NRF24L01+
// in a star network with one hub and six nodes
//
//
//This sketch is a modification from a video on the ForceTronics YouTube Channel,
//which code was leveraged from http://maniacbug.github.io/RF24/starping_8pde-example.html
//This sketch is free to the public to use and modify at your own risk

#include <LiquidCrystal.h> // for use with the SainSmart LCD Keypad Sheild V1.0
#include <SPI.h> // used to communicate with the nRF24L01+
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

// Arduino UNO pins associated with the LCD shield
#define rs 8
#define en 9
#define d4 4
#define d5 5
#define d6 6
#define d7 7
// create and instance lcd for the LCD shield
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Pins in use
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            145  // up
#define DOWN_10BIT_ADC          329  // down
#define LEFT_10BIT_ADC          505  // left
#define SELECT_10BIT_ADC        741  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  //

//State Machine Labels        State
#define STATE_READY           0
#define STATE_SET_STARTTEMP   1
#define STATE_SET_SPREAD      2
#define STATE_SET_OFFSETTEMP  3
#define STATE_COMP_STATS      4
#define STATE_COMP_POS        5
#define STATE_LIGHT_STATS     6
#define STATE_LIGHT_CAL       7
#define STATE_TEMP_STATS      8
#define STATE_AC_TEST         9
#define STATE_REHEARSAL       10
#define STATE_RESET           11
#define STATE_SET_INCREMENT   12
#define STATE_SET_CYCLES      13
#define STATE_END             14

/*--------------------------------------------------------------------------------------
  Variables
  --------------------------------------------------------------------------------------*/

// Wireless Communications
#define CHAN 108 // set the channel used by the 2.4Ghz transceivers - MUST BE THE SAME ON ALL NODES
const int pinCE = 2; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 3; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
RF24 radio(pinCE, pinCSN); // Declare object from nRF24 library (Create your wireless SPI)

//Create up to 6 pipe addresses P0 - P5;  the "LL" is for LongLong type
// RF24 library doesn't enforce, but asks that all but the two least significant bits share the same value
const uint64_t rAddress[] = {
  0xB3B4B5B6AALL,
  0xB3B4B5B6BBLL,
  0xB3B4B5B6CCLL,
  0xB3B4B5B6DDLL,
  0xB3B4B5B6EELL,
  0xB3B4B5B6FFLL
  //--------^^ least two significant bits are unique, all the rest are the same
};

//for the stateMachine
int state = 0;
int thisState = 0;
int lastState = 0;
boolean AC = false;
boolean play = false;

//button handling
byte buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
byte buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events
byte button = 0;

//Temperature
float tempLast[4]; // stores the last read value from each of the sensors
int tempCounts = 0; // used for averaging last several readings
int avgTempCounts = 0; // used for averaging the last several average readings
float avgTemp = 0; // holds the average temp value
float minTemp;
float maxTemp;
float offsetTemp = 0;
const int tempSamples = 16; //number of temp samples to average
const int avgTempSamples = 32; //number of temp averages to average
float tempLastTen[tempSamples]; // buffer for a rolling average of last several temp readings
float avgTemps[avgTempSamples]; // buffer for a rolling average of last several average temp readings
float startTemp = 71; // temperature the composition starts at
float spread = 2; // the spread in degrees betwee the min and max temp
float increment = 0; // the amount in degrees that the composition shifts upwards by each cycle

//Composition logic
int compCycles = 20; // number of cycles in the composition
boolean maxReached = false; // indicates whether the max has been reached, used to turn on AC.
boolean theEnd = false; // indicates when the end of the composition has been reached
boolean theStart = true; // flag to indicate the start of the performance
boolean reset = false; // flag used to trigger the reset to the start of the composition
int compCount = 0; // counter to track where we are in the composition
int oldCount = 0; // used to remember the last composition count
int newCount = 0; // used to update the last composition count

//Simulating temerature in rehearsal()
float testTemp = 75; // value used by the rehearsal state to simulate the
float incTemp = 0.025; // value to increment the testTemp by... the smaller, the longer the rehearsal takes to complete
int dirTemp = 1; // flag that changes the direction of the testTemp

//Lights
int maxBrightness = 180; // default value for the maximum brightness of the dimmers
int minBrightness = 80; // default value for the minimum brightness of the dimmers
float percentage = 0; // place holder for the percentage of brightness relative to the usable range of the dimmer
byte storeBrightness; // placeholder used to update the light brightness during calibration mode
boolean lightMode = true; // used to toggle between calibrating the max and min brightness of the dimmer

//Execution timing
unsigned long timer = 0; // used as in-loop timer to trigger the program() execution
unsigned int updateTime = 250; // interval of program execution in milliseconds

//The composition as a table of max and min temeratures
int composition[24][2];

void setup()
{
//  Serial.begin(115200);
//  Serial.println();
  createComposition(); // generates the composition max and min values based on startTemp, spread, increment, and compCycles
  
  updateMinMaxTemp(); // updates the min and max temp values from the composition array
  
  radio.begin();  //Start the nRF24 module
  radio.setPALevel(RF24_PA_HIGH);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(CHAN);          // the higher channels tend to be more "open"
  // Open up to six pipes for PRX to receive data
  for (int i = 0 ; i < 6 ; i++) {
    radio.openReadingPipe(i, rAddress[i]);
  }

  lcd.begin(16, 2);
  
  // startup message
  lcd.clear();
  lcd.print("New Max");
  lcd.setCursor(0, 1);
  lcd.print("By M. Hollander");
  delay(1000);
  lcd.clear();
  // end startup message
  
  setAC(0); // turn off the AC
  setLights(255); // turn the lights all the way up
  
  radio.startListening(); // start listening for readings from the temperature sensors
  
}

void loop() {

  getTemp(); // always scanning the tranceiver for incoming data

  if (millis() - timer >= updateTime) { // timed update of the composition
    timer = millis();
    avgTemp = getAvgTemp(); // update the average temperature value
    stateMachine(); // executes the program on the interval set but updateTime in milliseconds.
  }

}

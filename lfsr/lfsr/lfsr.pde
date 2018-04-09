import controlP5.*; //Hooray GUIs!
import processing.serial.*; //Used to bark orders at the Arduino

Serial arduino;

ControlP5 GUI;

RadioButton r1t1;
RadioButton r1t2;

RadioButton r2t1;
RadioButton r2t2;

RadioButton r3t1;
RadioButton r3t2;


void setup(){
  
  //dev mode
  size(1920,1080);
  
  //performance mode
  //fullScreen();
  
  //use this to list all serial devices
  //printArray(Serial.list());
  
  //my arduino nano is on port 5: [5] "/dev/tty.wchusbserial1410"
  arduino = new Serial(this, Serial.list()[5], 115200);
  
  setupGUI();
  
}

void draw(){
  background(0);
}

void serial2Arduino(byte addr, byte val){
  arduino.write(addr);
  arduino.write(val);
}

// Setting up Serial
//Modified from the original example by Example by Tom Igoe


//we need to import the Processing Serial library
import processing.serial.*;

//then declare a port
Serial arduinoPort;

long counter;
long output;
ArrayList <Light> lights;
ArrayList<Ring> rings;

void setup() {
  size(800, 600);
  frameRate(30);

  lights = new ArrayList<Light>();
  for (int i = 0; i < 32; i++) {
    lights.add(new Light(i));
  }

  rings = new ArrayList<Ring>();

  counter = 0xFF; //used for testLights()

  printArray(Serial.list()); // List all the available serial ports:
  arduinoPort = new Serial(this, Serial.list()[2], 115200); // Open the port you are using at the rate you want:
}

// Send a capital A out the serial port:
void draw() {
  background(0);

  for (int i = rings.size() - 1; i >= 0; i--) {
    Ring ring = rings.get(i);
    if (ring.r > sqrt(pow(width,2)+pow(height,2))) {
      rings.remove(i);
    } else {
      ring.update();
    }
  }

  for (int i = lights.size()-1; i >=0; i--) {
    boolean[] intersection = new boolean[rings.size()];
    Light light = lights.get(i);
    for (int j = rings.size() - 1; j >= 0; j--) {
      Ring ring = rings.get(j);
      if (checkIntersection(ring.pos, ring.r, light.pos, light.r)) {
        intersection[j] = true;
      } else {
        intersection[j] = false;
      }
    }
    boolean ag = false;
    for(int k = 0 ; k < intersection.length;k++){
      ag |= intersection[k];
    }
    light.toggle = ag;

    light.update();
  }



  output=0;
  for (int i = lights.size()-1; i >=0; i--) {
    Light light = lights.get(i);
    output|=int(light.lightOn) << i;
  }

  sendData(output);
}

boolean checkIntersection(PVector _ring, float _ringr, PVector _light, float _lightr) {
  float dist = PVector.dist(_ring, _light);
  return (dist - _ringr) < _lightr && _ringr < (dist + _lightr);
}

void mousePressed() {

  PVector mouse = new PVector(mouseX, mouseY);
  rings.add(new Ring(mouse));
  //for (int i = lights.size()-1; i >=0; i--) {
  //  Light light = lights.get(i);
  //  if (PVector.dist(mouse, light.pos) < light.r) {
  //    light.toggle=!light.toggle;
  //  }
  //}
}

void sendData(long output) {
  byte data = 0;
  arduinoPort.write(0xA0); // send sync byte, Arduino is looking for 0xA0
  for (int i = 0; i < 4; i++) {
    data = byte((output >> (i*8)) & 0xFF);
    arduinoPort.write(data);
  }
}

void testLights() {
  byte data = 0;
  arduinoPort.write(0xA0);
  for (int i = 0; i < 4; i++) {
    data = byte((counter >> (i*8)) & 0xFF);
    arduinoPort.write(data);
  }
  println(counter);
  //counter = int(random(0xFF)) << 24 |int(random(0xFF)) << 16 |int(random(0xFF)) << 8 | int(random(0xFF));
  //counter++;
  //counter = counter << 1 ^ ((counter >> 31) & 1);
}

class Light {
  PVector pos;
  float r;
  int id;
  boolean vis, mouseOver, lightOn, toggle;

  Light(int _id) {
    pos = new PVector(random(width), random(height));
    r = 50;
    vis = true;
    id = _id;
    mouseOver=false;
    toggle=false;
    lightOn = false;
  }

  void update() {

    PVector mouse = new PVector(mouseX, mouseY);
    if (PVector.dist(mouse, pos) < r) {
      mouseOver=true;
    } else {
      mouseOver = false;
    }

    lightOn = toggle | mouseOver;
    strokeWeight(1);
    if (lightOn) {
      stroke(255);
      fill(0);
    } else {
      stroke(0);
      fill(255);
    }

    ellipse(pos.x, pos.y, 2*r, 2*r);
    fill(0);
    text(id, pos.x, pos.y);
  }

  boolean inRadius(PVector _pos) {
    if (PVector.dist(_pos, pos) < r) {
      return true;
    } else {
      return false;
    }
  }

  boolean inRadius(float _x, float _y) {
    PVector _pos = new PVector(_x, _y);
    if (PVector.dist(_pos, pos) < r) {
      return true;
    } else {
      return false;
    }
  }
}

class Ring {
  PVector pos;
  float r;
  float rate;
  Ring (float _x, float _y) {
    pos = new PVector(_x, _y);
    r=0;
    rate=5;
  }

  Ring (PVector _pos) {
    pos = _pos;
    r=0;
    rate=5;
  }

  void update() {
    strokeWeight(2);
    stroke(255);
    noFill();
    ellipse(pos.x, pos.y, 2*r, 2*r);
    r+=rate;
  }
}
/*
Light Control for Crash and Flow 2.0 by Krussia
 by Phillip David Stearns 2018
 
 Communicates to an Arduino via serial
 Companion Arduino sketch is CrashAndFlow.ino
 
 Key Mappings:
 
 E                    toggle editMode
 
 ALT+Click            Add a Light to lights (editMode only)
 SHIFT+Click+Drag     Move selected Light(s) (editMode only)
 DELETE or BACKSPACE  Remove Light in reverse order of placement (editMode only)
 S                    Save positions of lights to settings.csv
 L                    Clears current light configuration and loads lights from settings.csv (editMode only)
 
 r                    Creates a Ring that expands outwards to trigger lights
 UP                   Creates a Line that moves from bottom to top of screen to trigger lights
 DONW                 Creates a Line that moves from top to bottom of screen to trigger lights
 LEFT                 Creates a Line that moves from left to right of screen to trigger lights 
 RIGHT                Creates a Line that moves from right to left of screen to trigger lights
 z                    Turns random light flickering on and off (temporary)
 x                    Turns random light toggle switches on and off (holds after press)
 c                    Clears light toggles
 SPACE                Turns all light toggles on/off
 
 */

import processing.serial.*;
Serial arduinoPort;

int lightCount;
int lightsMax = 32;
ArrayList <Light> lights;
ArrayList<Ring> rings;
ArrayList<Line> lines;
boolean editMode;
boolean allOn;
/////////////////////////////////SETUP///////////////////////////////

void setup() {
  size(800, 600);
  //fullScreen();
  background(0);
  frameRate(30);
  lightCount=0;
  editMode=true; //0 = edit, 1 = perform
  allOn = false;
  lights = new ArrayList<Light>();
  rings = new ArrayList<Ring>();
  lines = new ArrayList<Line>();
  printArray(Serial.list()); // List all the available serial ports:
  arduinoPort = new Serial(this, Serial.list()[2], 115200); // Open the port you are using at the rate you want:
}

/////////////////////////////////MAIN LOOP///////////////////////////////

void draw() {
  background(0);
  if (editMode) {
    updateLights();
    sendData(packLights());
  } else {
    updateRings();
    updateLines();
    updateLights();
    sendData(packLights());
  }
}

/////////////////////////////////FUNCTIONS///////////////////////////////

void updateRings() {
  for (int i = rings.size() - 1; i >= 0; i--) {
    Ring ring = rings.get(i);
    if (ring.isDone()) {
      rings.remove(i);
    } else {
      ring.update();
    }
  }
}

void updateLines() {
  for (int i = lines.size() - 1; i >= 0; i--) {
    Line line = lines.get(i);
    if (line.isDone()) {
      lines.remove(i);
    } else {
      line.update();
    }
  }
}

void updateLights() {
  for (int i = lights.size()-1; i >=0; i--) {
    Light light = lights.get(i);
    light.update();
  }
}

long packLights() {
  long output=0;
  for (int i = lights.size()-1; i >=0; i--) {
    Light light = lights.get(i);
    output|=int(light.isOn()) << i;
  }
  return output;
}

void sendData(long output) {
  byte data = 0;
  arduinoPort.write(0xA0); // send sync byte, Arduino is looking for 0xA0
  for (int i = 0; i < 4; i++) {
    data = byte((output >> (i*8)) & 0xFF);
    arduinoPort.write(data);
  }
}

/////////////////////////////////SAVE & RECALL SETTINGS///////////////////////////////


// output positions of lights to settings.csv
void saveSettings() {
  Table settings = new Table();
  settings.addColumn("id");
  settings.addColumn("x");
  settings.addColumn("y");

  for (int i = 0; i < lights.size(); i++) {
    Light light = lights.get(i);
    TableRow newRow = settings.addRow();
    newRow.setInt("id", light.id);
    newRow.setFloat("x", light.pos.x);
    newRow.setFloat("y", light.pos.y);
  }
  saveTable(settings, "settings.csv");
}

//load positions of lights into ArrayList
void loadSettings() {

  //initialize ArrayList
  lights = new ArrayList<Light>();
  lightCount=0;

  Table settings = loadTable("settings.csv", "header");

  for (TableRow row : settings.rows()) {
    lights.add(new Light(new PVector(row.getFloat("x"), row.getFloat("y")), row.getInt("id")));
    lightCount++;
  }
}
/////////////////////////////////INTERACTION///////////////////////////////


void keyPressed() {

  switch(key) {

  case 'S': // Save position of lights into settings.csv
    saveSettings();
    break;
  case 'L':
    if (editMode)loadSettings();
    break;
  case 'E': // TOGGLE EDITMODE
    editMode = !editMode;
    println("Edit mode: "+editMode);
    break;
  case 'r': //ADD A RING
    rings.add(new Ring(mouseX, mouseY));
    break;
  case 'z':
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      light.random=!light.random;
    }
    break;
  case 'x':
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      light.toggle=!boolean(int(random(10)));
    }
    break;
  case 'c':
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      light.toggle=false;
    }
    break;
  case 'i':
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      light.invert=!light.invert;
    }
    break;
  }

  // FOR ADDING LINES 
  switch(keyCode) {
  case UP:
    lines.add(new Line(true, false));
    break;
  case DOWN:
    lines.add(new Line(true, true));
    break;
  case LEFT:
    lines.add(new Line(false, false));
    break;
  case RIGHT:
    lines.add(new Line(false, true));
    break;
  }

  if (editMode) {
    //Use DELETE or BACKSPACE key's to delete the last light added
    if (key == DELETE || key == BACKSPACE && lightCount > 0) {
      lights.remove(lightCount-1);
      lightCount--;
    }

    //logic for selecting/deselecting Light objects
    if (keyPressed && keyCode == SHIFT) {
      for (int i = lights.size() - 1; i >= 0; i--) {
        Light light = lights.get(i);
        if (light.inRadius(mouseX, mouseY)) {
          light.selected = true;
        }
      }
    }
  }
} // keyPressed

void keyReleased() {

  //logic for selecting/deselecting Light objects
  if (keyCode == SHIFT) {
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      if (light.isSelected()) {
        light.selected = false;
      }
    }
  }
} // keyReleased

void mousePressed() {

  PVector mouse = new PVector(mouseX, mouseY);

  if (editMode) { // logic for edit mode interaction with Light objects
    // logic for adding Light objects
    if (keyPressed && keyCode == ALT) {
      boolean lightClicked = false;
      for (int i = lights.size() - 1; i >= 0; i--) {
        Light light = lights.get(i);
        if (light.inRadius(mouse)) {
          lightClicked |= true;
        }
      } 
      if (!lightClicked) { // don't add a new Light object on top of an old one
        if (lightCount<32) {
          lights.add(new Light(mouse, lightCount));
          lightCount++;
        }
      }
    } else if (!(keyPressed && keyCode == SHIFT)) { // logic for toggling Light objects on and off
      for (int i = lights.size() - 1; i >= 0; i--) {
        Light light = lights.get(i);
        if (light.inRadius(mouse)) {
          light.toggle = !light.toggle;
        }
      }
    }
  }
} // mousePressed

void mouseMoved() {

  //logic for selecting/deselecting Light objects
  if (editMode && keyPressed && keyCode == SHIFT) {
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      if (light.inRadius(mouseX, mouseY)) {
        light.selected = true;
      } else if (light.isSelected()) {
        light.selected = false;
      }
    }
  }
} // mouseMoved

void mouseDragged() {

  //logic for moving Light objects that have been selected
  if (editMode) {
    for (int i = lights.size() - 1; i >= 0; i--) {
      Light light = lights.get(i);
      if (light.isSelected()) {
        light.pos.x+=mouseX-pmouseX;
        light.pos.y+=mouseY-pmouseY;
      }
    }
  }
} // mouseDragged


/////////////////////////////////CLASSES///////////////////////////////

//-------------LIGHT CLASS-------------

class Light {
  PVector pos;
  float r = 50;
  int id;
  boolean selected = false, toggle = false, logic = false, random = false, invert = false;

  Light(PVector _pos, int _id) {
    pos = _pos;
    r = 50;
    id = _id;
  }

  void applyLogic(boolean _logic) {
    logic |= _logic;
  }

  void update() {

    logic = false;

    ringLogic(rings);
    lineLogic(lines);
    mouseLogic();
    if (random) randomLogic();


    display();
  }

  void display() {
    color txt, stroke, fill;
    if (!selected) {
      if (isOn()) {
        stroke = 0;
        fill = 255;
        txt = 0;
      } else {
        stroke = 255;
        fill = 0;
        txt = 255;
      }
    } else {
      if (isOn()) {
        stroke = 0;
        fill = 127;
        txt = 0;
      } else {
        stroke = 127;
        fill = 0;
        txt = 127;
      }
    }

    strokeWeight(1);
    stroke(stroke);
    fill(fill);
    ellipse(pos.x, pos.y, 2*r, 2*r);
    fill(txt);
    text(id+1, pos.x, pos.y);
  }

  boolean inRadius(PVector _pos) {
    if (PVector.dist(_pos, pos) < r) {
      return true;
    } else {
      return false;
    }
  }

  boolean inRadius(float _x, float _y) {
    if (PVector.dist(new PVector(_x, _y), pos) < r) {
      return true;
    } else {
      return false;
    }
  }

  boolean intersectX(float _x) {
    if (PVector.dist(new PVector(_x, pos.y), pos) < r) {
      return true;
    } else {
      return false;
    }
  }

  boolean intersectY(float _y) {
    if (PVector.dist(new PVector(pos.x, _y), pos) < r) {
      return true;
    } else {
      return false;
    }
  }

  boolean isSelected() {
    return selected;
  }

  boolean isOn() {
    if (invert) {
      return !(logic || toggle);
    } else {
      return (logic || toggle);
    }
  }

  boolean ringIntersect(Ring _ring) {
    float dist = PVector.dist(_ring.pos, pos);
    return (dist - _ring.r) < r && _ring.r < (dist + r);
  }

  void ringLogic(ArrayList<Ring> _rings) {
    for (int j = _rings.size() - 1; j >= 0; j--) {
      Ring ring = _rings.get(j);
      if (ringIntersect(ring)) {
        applyLogic(true);
      } else {
        applyLogic(false);
      }
    }
  }

  void lineLogic(ArrayList<Line> _lines) {
    for (int j = _lines.size() - 1; j >= 0; j--) {
      Line line = _lines.get(j);
      if (line.hv) {
        if (intersectY(line.pos.y)) {
          applyLogic(true);
        } else {
          applyLogic(false);
        }
      } else {
        if (intersectX(line.pos.x)) {
          applyLogic(true);
        } else {
          applyLogic(false);
        }
      }
    }
  }

  void mouseLogic() {
    if (inRadius(mouseX, mouseY)) {
      applyLogic(true);
    } else {
      applyLogic(false);
    }
  }

  void randomLogic() {
    applyLogic(!boolean(int(random(10))));
  }
}


//-------------RING CLASS-------------

class Ring {
  PVector pos;
  float r = 0;
  float rate = 15;

  Ring(float _x, float _y) {
    pos = new PVector(_x, _y);
  }

  Ring(PVector _pos) {
    pos = _pos;
  }

  void update() {
    strokeWeight(2);
    stroke(255);
    noFill();
    ellipse(pos.x, pos.y, 2*r, 2*r);
    r+=rate;
  }

  boolean isDone() {
    return (r > sqrt(pow(width, 2)+pow(height, 2)));
  }
}

//-------------LINE CLASS-------------

class Line {
  PVector pos = new PVector(0, 0);
  float rate = 20;
  boolean hv;  // horizontal or vertical orientation, true = horizontal, false = vertical
  boolean dir; // direction of movement, true left to right / top to bottom, false right to left / bottom to top

  Line(boolean _hv, boolean _dir) {
    hv = _hv;
    dir = _dir;

    if (hv && dir || !hv && dir) {
      pos.y = 0;
      pos.x = 0;
    } else if (hv && !dir) {
      pos.y = height-1;
      pos.x = 0;
    } else if (!hv && !dir) {
      pos.y = 0;
      pos.x = width-1;
    }
  }

  void update() {
    strokeWeight(2);
    stroke(255);
    noFill();

    if (hv && dir || !hv && dir) {
      pos.y += rate;
      pos.x += rate;
    } else if (hv && !dir) {
      pos.y -= rate;
      pos.x = 0;
    } else if (!hv && !dir) {
      pos.y = 0;
      pos.x -= rate;
    }

    if (hv) {
      line(0, pos.y, width-1, pos.y);
    } else {
      line(pos.x, 0, pos.x, height-1);
    }
  }

  boolean isDone() {
    if (hv && dir) {
      return pos.y >= height-1;
    } else if (hv && !dir) {
      return pos.y <= 0;
    } else if (!hv && dir) {
      return pos.x >= width-1;
    } else if (!hv && !dir) {
      return pos.x <= 0;
    } else {
      return true;
    }
  }
}
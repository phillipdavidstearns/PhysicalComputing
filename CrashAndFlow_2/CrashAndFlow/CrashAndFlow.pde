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

boolean editMode;
boolean demoMode;

//variables associated with Light
ArrayList <Light> lights;
int lightCount;
int lightsMax = 32;
boolean allOn;
boolean random;
boolean caOn;
boolean inverted;
boolean lfsrOn;

//variables associated with Group
ArrayList <Group> groups;
int groupCount;
int groupsMax = 6;
boolean[] group; //holds logic for toggling groups on and off
String[] groupNames = {"KEYS", "DRUMS", "VOCALS", "GUITAR", "BASS"};

ArrayList <Ring> rings;
ArrayList <Line> lines;

CA ca;
LFSR lfsr;

color bgcolor;

//variables for calculating and drawing the grid
float stageWidth =24; // in feet
float stageDepth=12; // in feet
float wallHeight=6; // in feet
float gridWidth; // in pixels
float gridHeight; // in pixels
float gridSize; // in pixels
float padding = 25; // in pixels
int heavy = 4;
int medium = 2;
int light = 1;
float gridXstart;
float gridXend;
float gridYstart;
float gridYend;
float textX;
float columnLabelY;
float rowLabelX;

/////////////////////////////////SETUP///////////////////////////////

void setup() {
  //size(800, 600);
  fullScreen();
  frameRate(30);

  computeGrid();
  drawGrid();

  editMode=false; //0 = edit, 1 = perform
  demoMode=false;

  lights = new ArrayList<Light>();
  lightCount=0;
  allOn = false;
  inverted = false;
  random = false;
  caOn = false;
  lfsrOn = false;

  groups = new ArrayList<Group>();
  groupCount=0;
  group = new boolean[groupsMax];

  rings = new ArrayList<Ring>();
  lines = new ArrayList<Line>();

  ca = new CA();
  lfsr = new LFSR();

  loadSettings();
  printArray(Serial.list()); // List all the available serial ports:
  arduinoPort = new Serial(this, Serial.list()[2], 115200); // Open the port you are using at the rate you want
}

/////////////////////////////////MAIN LOOP///////////////////////////////

void draw() {

  if (editMode) {
    bgcolor = 63;
  } else if (inverted) {
    bgcolor = 127;
  } else {
    bgcolor = 0;
  }

  background(bgcolor);

  drawGrid();

  if (!editMode) {
    updateRings();
    updateLines();
    ca.update();
    lfsr.update();
  }
  updateGroups();
  updateLights();
  sendData(packLights());
}

/////////////////////////////////FUNCTIONS///////////////////////////////

void computeGrid() {
  if (width/stageWidth < height/(stageDepth+wallHeight)) {
    gridSize = (width-(2*padding))/stageWidth;
  } else {
    gridSize = (height-(2*padding))/(stageDepth+wallHeight);
  }
  gridWidth = gridSize*stageWidth;
  gridHeight = gridSize*(stageDepth+wallHeight);
  gridXstart = (width - gridWidth)/2;
  gridYstart = (height - gridHeight)/2;
  columnLabelY=height-gridYstart+(gridSize/4);
  rowLabelX=gridXstart-(gridSize/4);
  gridYend = height-gridYstart;
  gridXend = width-gridXstart;
}

void drawGrid() {

  //Grid colors
  if (editMode) {
    fill(255);  
    stroke(255);
  } else if (inverted) {
    fill(63);
    stroke(63);
  } else {
    fill(127);
    stroke(127);
  }

  textAlign(CENTER, CENTER);

  //draw vertical lines
  for (int x = 0; x <= stageWidth; x++) {
    //set line weight
    if (x==0 || x == stageWidth) {
      strokeWeight(heavy);
    } else if (x % 2 == 0) {
      strokeWeight(medium);
    } else {
      strokeWeight(light);
    }
    //label grid
    float columnLabelX=gridXstart+(gridSize*x)+(gridSize/2);
    if (x < stageWidth) text(x+1, columnLabelX, columnLabelY);
    float boxX = gridXstart+(gridSize*x);
    line(boxX, gridYstart, boxX, gridYend);
    strokeWeight(0);
    line(boxX, 0, boxX, height);
  }

  //draw horizontal lines
  for (int y = 0; y <= wallHeight+stageDepth; y++) {
    //set line weight
    if (y==wallHeight || y==0 || y == wallHeight+stageDepth) {
      strokeWeight(heavy);
    } else if (y % 2 == 0) {
      strokeWeight(medium);
    } else {  
      strokeWeight(light);
    }
    //label grid
    float rowLabelY = gridYstart+(gridSize*y)+(gridSize/2);
    if (y < stageDepth+wallHeight) text(y+1, rowLabelX, rowLabelY);
    float boxY = gridYstart+(gridSize*y);
    line(gridXstart, boxY, gridXend, boxY);
    strokeWeight(0);
    line(0, boxY, width, boxY);
  }
}

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
  for (Light light : lights) {
    light.update();
  }
}

void updateGroups() {
  for (Group group : groups) {
    group.update();
  }
}

long packLights() {
  long packed=0;
  for (int i = lights.size()-1; i >=0; i--) {
    Light light = lights.get(i);
    packed |= int(light.isOn()) << i;
  }
  return packed;
}

void sendData(long _output) {
  byte data = 0;
  arduinoPort.write(0xA0); // send sync byte, Arduino is looking for 0xA0
  for (int i = 0; i < 4; i++) {
    data = byte((_output >> (i*8)) & 0xFF);
    arduinoPort.write(data);
  }
}

/////////////////////////////////SAVE & RECALL SETTINGS///////////////////////////////

// output positions of lights to settings.csv
void saveSettings() {
  Table settings = new Table();

  //save lights
  settings.addColumn("id");
  settings.addColumn("x");
  settings.addColumn("y");
  for (int i = 0; i < group.length; i++) {
    settings.addColumn("group"+(i+1));
  }


  for (Light light : lights) {
    TableRow newRow = settings.addRow();
    newRow.setInt("id", light.id);
    newRow.setFloat("x", light.pos.x);
    newRow.setFloat("y", light.pos.y);
    for (int i = 0; i < group.length; i++) {
      newRow.setInt("group"+(i+1), int(light.memberOf(i)));
    }
  }
  saveTable(settings, "lights.csv");

  //save groups
  settings = new Table();
  settings.addColumn("name");
  settings.addColumn("x");
  settings.addColumn("y");

  for (Group group : groups) {
    TableRow newRow = settings.addRow();
    newRow.setString("name", group.name);
    newRow.setFloat("x", group.pos.x);
    newRow.setFloat("y", group.pos.y);
  }
  saveTable(settings, "groups.csv");
}

//load positions of lights into ArrayList
void loadSettings() {
  Table settings = loadTable("lights.csv", "header");
  if (settings != null) {
    //initialize ArrayList
    lights = new ArrayList<Light>();
    lightCount=0;

    for (int i = 0; i < settings.getRowCount(); i++) {
      TableRow row = settings.getRow(i);
      lights.add(new Light(new PVector(row.getFloat("x"), row.getFloat("y")), row.getInt("id")));
      for (int j = 0; j < group.length; j++) {
        lights.get(i).member[j] = boolean(row.getInt("group"+(j+1)));
      }
      lightCount++;
    }
  } else {
    println("Couldn't load light presets from file.");
  }

  if (settings != null) {
    groups = new ArrayList<Group>();
    groupCount=0;

    settings = loadTable("groups.csv", "header");

    for (TableRow row : settings.rows()) {
      groups.add(new Group(new PVector(row.getFloat("x"), row.getFloat("y")), row.getString("name")));
      groupCount++;
    }
  } else {
    println("Couldn't load group presets from file.");
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
  case 'z': // sparkle mode!
    random=!random;
    break;
  case 'x': // turns on a random selection of lights
    for (Light light : lights) {
      light.toggle=!boolean(int(random(10)));
    }
    break;
  case 'c': // turns off all lights!
    allOn=false;
    for (Light light : lights) {
      light.toggle=false;
    }
    for (int i  = 0; i < group.length; i++) {
      group[i]=false;
    }
    break;
  case 'i': // inverts the state of all lights
    inverted = !inverted;
    break;

    //cellular automata interaction  
  case 'a': // toggle cellular automata display
    caOn = !caOn; 
    break;
  case 's':
    ca.randomizeRules();
    break;
  case 'd':
    ca.randomizeRegister();
    break;
  case 'f':
    ca.reg=1;
    break;
  case ',': // slows down movement of CA animation
    ca.incRate();
    break;
  case '.': // speeds up movement of CA animation
    ca.decRate();
    break;

    //lfsr rules
  case 'l':
    lfsrOn = !lfsrOn;
    break;
  case 'k': //toggle recirculation vs linear feedback shift register mode
    lfsr.mode = !lfsr.mode;
    break;
  case 'j':
    lfsr.randomize();
    break;
  case 'J':
    lfsr.reg = 1;
    break;
  case ';': // slow it down
    lfsr.incRate();
    break;
  case '\'': // speed it up
    lfsr.decRate();
    break;
  case ':':
    lfsr.decTap2();
    break;
  case '"':
    lfsr.incTap2();
    break;
  }

  if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6') {
    int keyNumber=0;
    switch (key) {
    case '1':
      keyNumber=1;
      break;
    case '2':
      keyNumber=2;
      break;
    case '3':
      keyNumber=3;
      break;
    case '4':
      keyNumber=4;
      break;
    case '5':
      keyNumber=5;
      break;
    case '6':
      keyNumber=6;
      break;
    }
    if (editMode) {
      for (Light light : lights) {
        light.selected = light.memberOf(keyNumber-1);
      }
    } else {
      group[keyNumber-1]=!group[keyNumber-1];
    }
  } else if (key == '!' || key == '@' || key == '#' || key == '$' || key == '%' || key == '^') {
    int keyNumber=0;
    switch (key) {
    case '!':
      keyNumber=1;
      break;
    case '@':
      keyNumber=2;
      break;
    case '#':
      keyNumber=3;
      break;
    case '$':
      keyNumber=4;
      break;
    case '%':
      keyNumber=5;
      break;
    case '^':
      keyNumber=6;
      break;
    }
    if (editMode) {
      for (Light light : lights) {
        light.member[keyNumber-1] = false;
      }
    } else {
      group[keyNumber-1]=!group[keyNumber-1];
    }
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

    // turning all lights on - to turn all on and off, press SPACE then 'c'  
  case 32:
    allOn=true;
    break;
  }

  if (editMode) {

    //Use DELETE or BACKSPACE key's to delete the last light added
    if (key == DELETE || key == BACKSPACE && lightCount > 0) {
      lights.remove(lightCount-1);
      lightCount--;
    }

    //logic for selecting/deselecting Light objects
    if (keyCode == SHIFT) {
      for (Light light : lights) {
        light.selected = light.inRadius(mouseX, mouseY);
      }
      for (Group group : groups) {
        group.selected = group.inRadius(mouseX, mouseY);
      }
    }
  }
} // keyPressed

void keyReleased() {
  PVector mouse = new PVector(mouseX, mouseY);
  //logic for selecting/deselecting Light objects
  if (editMode && keyCode == SHIFT || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6') {
    for (Light light : lights) {
      if (light.isSelected() || light.inRadius(mouse)) {
        light.selected = false;
      }
    }
    for (Group group : groups) {
      if (group.isSelected() || group.inRadius(mouse)) {
        group.selected = false;
      }
    }
  } else if (keyCode == 32) {
    allOn=false;
  }

  switch(key) {
  case '1': //KEYS
    if (editMode) { // displays which lights are assigned to group
      for (Light light : lights) {
        light.selected = false;
      }
    } else { // turns on lights assigned to group
      group[0]=false;
    }
    break;
  case '2': //DRUMS
    if (editMode) {
      for (Light light : lights) {
        light.selected = false;
      }
    } else {
      group[1]=false;
    }
    break;
  case '3': //VOCALS
    if (editMode) {
      for (Light light : lights) {
        light.selected = false;
      }
    } else {  
      group[2]=false;
    }
    break;
  case '4': //GUITAR
    if (editMode) {
      for (Light light : lights) {
        light.selected = false;
      }
    } else {  
      group[3]=false;
    }
    break;
  case '5': //BASS
    if (editMode) {
      for (Light light : lights) {
        light.selected = false;
      }
    } else {  
      group[4]=false;
    }
    break;
  case '6': //Between Songs
    if (editMode) {
      for (Light light : lights) {
        light.selected = false;
      }
    } else {  
      group[5]=false;
    }
    break;
  }
} // keyReleased

void mousePressed() {

  PVector mouse = new PVector(mouseX, mouseY);

  if (editMode) { // logic for edit mode interaction with Light objects
    if (keyPressed && keyCode == ALT) { // logic for adding a Light 
      boolean lightClicked = false;
      for (Light light : lights) {
        lightClicked |= light.inRadius(mouse);
      } 
      if (!lightClicked && lightCount<32) { // don't add a new Light on top of an old one 
        lights.add(new Light(mouse, lightCount));
        lightCount++;
      }
    } else if (keyPressed && keyCode == CONTROL) { //logic for adding a Group
      boolean clicked = false;
      for (Group group : groups) {
        clicked |= group.inRadius(mouse);
      } 
      if (!clicked && groupCount<5) { // don't add a new Group on top of an old one 
        groups.add(new Group(mouse, groupNames[groupCount]));
        groupCount++;
      }
    } else if (keyPressed && (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6')) {
      int keyNumber=0;
      switch (key) {
      case '1':
        keyNumber=1;
        break;
      case '2':
        keyNumber=2;
        break;
      case '3':
        keyNumber=3;
        break;
      case '4':
        keyNumber=4;
        break;
      case '5':
        keyNumber=5;
        break;
      case '6':
        keyNumber=6;
        break;
      }
      for (Light light : lights) {
        if (light.inRadius(mouse)) {
          light.member[keyNumber-1] = !light.member[keyNumber-1];
          light.selected = light.member[keyNumber-1];
        }
      }
    } else if (!keyPressed) { // logic for toggling Light objects on and off
      for (Light light : lights) {
        if (light.inRadius(mouse)) light.toggle = !light.toggle;
      }
    }
  } else {
    rings.add(new Ring(mouse));
  }
} // mousePressed

void mouseMoved() {

  //logic for selecting/deselecting Light objects
  if (editMode && keyPressed && keyCode == SHIFT) {
    for (Light light : lights) {
      light.selected = light.inRadius(mouseX, mouseY);
    }
    for (Group group : groups) {
      group.selected = group.inRadius(mouseX, mouseY);
    }
  }
} // mouseMoved

void mouseDragged() {

  //logic for moving Light objects that have been selected
  if (editMode) {
    for (Light light : lights) {
      if (light.isSelected()) {
        light.pos.x+=mouseX-pmouseX;
        light.pos.y+=mouseY-pmouseY;
      }
    }
    for (Group group : groups) {
      if (group.isSelected()) {
        group.pos.x+=mouseX-pmouseX;
        group.pos.y+=mouseY-pmouseY;
      }
    }
  }
} // mouseDragged


/////////////////////////////////CLASSES///////////////////////////////

//-------------GROUP CLASS-------------

class Group {
  PVector pos;
  float r = 80;
  float d = 2*r;
  String name;
  boolean selected = false;

  Group(PVector _pos, String _name) {
    pos = _pos;
    name = _name;
  }

  void update() {
    display();
  }

  void display() {

    color txt, stroke, fill;

    if (!selected) {
      stroke = 127;
      fill = 0;
      txt = 255;
    } else {
      stroke = 127;
      fill = 63;
      txt = 127;
    }


    strokeWeight(2);

    stroke(stroke);
    fill(fill);
    ellipse(pos.x, pos.y, d, d);

    fill(txt);
    textAlign(CENTER, CENTER);
    text(name, pos.x, pos.y);
  }

  boolean isSelected() {
    return selected;
  }

  boolean inRadius(PVector _pos) {
    return (PVector.dist(_pos, pos) < r);
  }

  boolean inRadius(float _x, float _y) {
    return (PVector.dist(new PVector(_x, _y), pos) < r);
  }
}

//-------------LIGHT CLASS-------------

class Light {
  PVector pos;
  float r = 50;
  float d = 2*r;
  int id;
  boolean selected = false, toggle = false, logic = false;
  boolean[] member = new boolean[group.length];

  Light(PVector _pos, int _id) {
    pos = _pos;
    id = _id;
  }

  void applyLogic(boolean _logic) {
    logic |= _logic;
  }

  void update() {
    logic = false;
    applyLogic(toggle);
    ringLogic();
    lineLogic();
    groupLogic();
    if (editMode) mouseLogic();
    if (random) randomLogic();
    if (allOn) applyLogic(true);
    if (caOn) applyLogic(ca.cellState(id));
    if (lfsrOn) applyLogic(lfsr.bit(id));
    display();
  }

  void groupLogic() {
    for (int i = 0; i < group.length; i++) {
      applyLogic(member[i] & group[i]);
    }
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

    strokeWeight(2);

    stroke(stroke);
    fill(fill);
    ellipse(pos.x, pos.y, d, d);

    fill(txt);
    textAlign(CENTER, CENTER);
    text(id+1, pos.x, pos.y);
  }

  boolean inRadius(PVector _pos) {
    return (PVector.dist(_pos, pos) < r);
  }

  boolean inRadius(float _x, float _y) {
    return (PVector.dist(new PVector(_x, _y), pos) < r);
  }

  boolean isSelected() {
    return selected;
  }

  boolean isOn() {
    return inverted ^ logic;
  }

  boolean memberOf(int _group) {
    return member[_group];
  }


  void ringLogic() {
    for (Ring ring : rings) {
      float dist = PVector.dist(ring.pos, pos);
      applyLogic((dist - ring.r) < r && ring.r < (dist + r));
    }
  }

  void lineLogic() {
    for (Line line : lines) {
      if (line.hv) {
        applyLogic(PVector.dist(new PVector(pos.x, line.pos.y), pos) < r);
      } else {
        applyLogic(PVector.dist(new PVector(line.pos.x, pos.y), pos) < r);
      }
    }
  }

  void mouseLogic() {
    applyLogic(inRadius(mouseX, mouseY));
  }

  void randomLogic() {
    applyLogic(!boolean(int(random(10))));
  }
}


//-------------RING CLASS-------------

class Ring {
  PVector pos;
  float r = 0;
  float rate = 25;

  Ring(float _x, float _y) {
    pos = new PVector(_x, _y);
  }

  Ring(PVector _pos) {
    pos = _pos;
  }

  void update() {
    strokeWeight(2);
    if (!inverted) {
      stroke(255);
    } else {
      stroke(0);
    }
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
  float rate = 25;
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
    if (!inverted) {
      stroke(255);
    } else {
      stroke(0);
    }
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

//-------------CA CLASS-------------
class CA {
  int reg = 1;
  byte rules = 0;
  int bitDepth = lightsMax;
  int rate = 4;

  CA() {
    randomizeRules();
  }

  void randomizeRules() {
    rules = byte(int(random(pow(2, 8))));
  }

  void randomizeRegister() {
    reg = int(random(pow(2, bitDepth)));
  }

  void update() { 
    if (frameCount % rate == 0) reg = applyRules();
  }

  void incRate() {
    if (rate < 10) rate++;
  }
  void decRate() {
    if (rate > 1) rate--;
  }


  int applyRules() {
    int result = 0;
    for (int i = 0; i < bitDepth; i++) {
      int state = 0;
      for (int n = 0; n < 3; n++) {
        int coord = ((i + bitDepth + (n-1)) % bitDepth);
        state |= (reg >> coord & 1) << (2-n);
      }  
      result |= ((rules >> state) & 1) << i;
    }
    return result;
  }

  boolean cellState(int _cell) {
    return boolean(reg >> _cell & 1);
  }
}

//-------------LFSR CLASS-------------

class LFSR {
  int reg;
  int rate = 4;
  int tap1 = 16; 
  int tap2 = 15;
  boolean tap1en;
  boolean tap2en;
  boolean mode; // false = recirc only, true = lfsr

  LFSR() {
    reg = 1;
  }

  void update() {
    if (frameCount % rate == 0) {

      if (mode) {
        tap1en = true;
        tap2en = true;
      } else {
        tap1en = true;
        tap2en = false;
      }

      reg = (reg << 1) | (int(tap1en) & (reg >> tap1 & 1)) ^ (int(tap2en) & (reg >> tap2 & 1));
    }
  }

  void incRate() {
    if (rate < 10) rate++;
  }
  void decRate() {
    if (rate > 1) rate--;
  }

  void incTap2() {
    if (tap2 < lightsMax-1) tap2++;
  }
  void decTap2() {
    if (tap2 > 0) tap2--;
  }

  void randomize() {
    for (int i = 0; i < lightsMax; i++) {
      reg |= int(random(2)) << i;
    }
  }

  void setMode(boolean _mode) {
    mode=_mode;
  }

  boolean bit(int _bit) {
    return boolean(reg >> _bit & 1);
  }
}
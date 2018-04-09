int radioW = 30;
int radioH = 30;
int radioX=20;
int radioY=20;
int radioPadding=5;
int count = 0;

void setupGUI() {


  GUI = new ControlP5(this);

  r1t1 = GUI.addRadioButton("r1t1")
    .setPosition(radioX, radioY+(radioH+radioPadding)*0)
    .setSize(radioW, radioH)
    ;
  setupRadioButtons(r1t1, 32);

  r1t2 = GUI.addRadioButton("r1t2")
    .setPosition(radioX, radioY+(radioH+radioPadding)*1)
    .setSize(radioW, radioH)
    ;
  setupRadioButtons(r1t2, 32);

  r2t1 = GUI.addRadioButton("r2t1")
    .setPosition(radioX, radioY+(radioH+radioPadding)*2)
    .setSize(radioW, radioH)
    ;
  setupRadioButtons(r2t1, 32);

  r2t2 = GUI.addRadioButton("r2t2")
    .setPosition(radioX, radioY+(radioH+radioPadding)*3)
    .setSize(radioW, radioH)
    ;
  setupRadioButtons(r2t2, 32);
  
  r3t1 = GUI.addRadioButton("r3t1")
    .setPosition(radioX, radioY+(radioH+radioPadding)*4)
    .setSize(radioW, radioH)
    ;
  setupRadioButtons(r3t1, 32);

  r3t2 = GUI.addRadioButton("r3t2")
    .setPosition(radioX, radioY+(radioH+radioPadding)*5)
    .setSize(radioW, radioH)
    ;
  setupRadioButtons(r3t2, 32);
  
  
}

void setupRadioButtons(RadioButton radio, int buttons) {
  radio.setItemsPerRow(buttons);
  for (int i = 0; i<32; i++) {
    String label = radio.getName()+i;
    radio.addItem(label, i);
    radio.getItem(i).getCaptionLabel().getStyle().movePadding(0, 0, 0, -3*radioW/4);
    radio.getItem(i).setLabel(nf(i));
  }
}

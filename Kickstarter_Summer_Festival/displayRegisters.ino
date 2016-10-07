void displayRegisters() {

  //load data into 4094 shift registers MSB to LSB
  for (int i = REGISTER_SIZE - 1 ; i >= 0 ; i--) {
    PORTB = ((REG1 >> i & 1) ^ ((REG2 >> ((REGISTER_SIZE - 1) - i)) & 1)) << 3;
    //pulse clock
    PORTB |= B00000100;
    PORTB = B00000000;
  }
  //pulse strobe to update 4094 shift registers
  PORTB = B00010000;
  PORTB = B00000000;
}

void updateRegisters(volatile unsigned long int output) {
  //load data into 4094 shift registers MSB to LSB
  for (int i = 32 ; i >= 0 ; i--) {
    PORTB = B00000000;
    PORTB = (output >> i & 1) << 3;
    //pulse clock
    PORTB |= B00010000;   
  }
  //pulse strobe to update 4094 shift registers
  PORTB = B00000100;
  PORTB = B00000000;
}

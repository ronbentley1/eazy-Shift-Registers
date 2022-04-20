//
//   Tutorial 1 - use of ez_SPI8 library
//   Setup and absolute addressing.
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

#define max_SIPOs  1  // one 1 SIPO for this tutorial
#define max_timers 0  // no timers required

// initiate the class for max SIPOs/timers required
SIPO8 my_SIPOs(max_SIPOs, max_timers);

int my_LEDs;  // used to keep the SIPO bank id

void setup() {
  Serial.begin(9600);
  // create a bank of 1 SIPO using create_bank function:
  // data pin, clock pin, latch pin, number of SIPO this bank
  my_LEDs = my_SIPOs.create_bank(8, 10, 9, 1); // data pin, clock pin, latch pin, number of SIPO this bank
  if (my_LEDs == create_bank_failure) {
    Serial.println(F("\nfailed to create bank"));
    Serial.flush();
    exit(0);
  }
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

void loop() {
  // start by setting all SIPO outputs to low (off)
  my_SIPOs.set_all_array_pins(LOW);
  my_SIPOs.xfer_array(LSBFIRST);
  do {
    //
    // assemble pattern 0b01010101 into the array
    my_SIPOs.set_array_pin(0, HIGH);
    my_SIPOs.set_array_pin(1, LOW);
    my_SIPOs.set_array_pin(2, HIGH);
    my_SIPOs.set_array_pin(3, LOW);
    my_SIPOs.set_array_pin(4, HIGH);
    my_SIPOs.set_array_pin(5, LOW);
    my_SIPOs.set_array_pin(6, HIGH);
    my_SIPOs.set_array_pin(7, LOW);
    my_SIPOs.xfer_array(MSBFIRST);  // now move array to physical SIPO and light up the LEDs
    delay(500);
    // assemble inverted pattern 0b10101010 into the array
    my_SIPOs.set_array_pin(0, LOW);
    my_SIPOs.set_array_pin(1, HIGH);
    my_SIPOs.set_array_pin(2, LOW);
    my_SIPOs.set_array_pin(3, HIGH);
    my_SIPOs.set_array_pin(4, LOW);
    my_SIPOs.set_array_pin(5, HIGH);
    my_SIPOs.set_array_pin(6, LOW);
    my_SIPOs.set_array_pin(7, HIGH);
    my_SIPOs.xfer_array(MSBFIRST);  // now move array to physical SIPO and light up the LEDs
    delay(500);
  } while (true);
}

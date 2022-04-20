//
//   Tutorial 5 - use of ez_SPI8 library,
//   Demonstration of SIPO bank interleaving - 8 x  SIPOs
//   each mapped to an individual bank but with the same 3-wire digital
//   pin microcontroller interface.
//
//   The sketch sets up each single SIPO bank with a different binary 8bit
//   (8 pin) pattern which is then xferred to the physical single SIPO IC.
//   The sketch mimics the strobe sketch by using bank interleaving.
//   To note is the small amount of SIPO8 library code that is used.
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

int bank_id;

#define Num_SIPOs   8
#define Num_timers  0

SIPO8 my_SIPOs(Num_SIPOs, Num_timers); // initiate the class for the tutorial

uint8_t bank_ids[Num_SIPOs]; // one bank_id per SIPO bank
uint8_t bank;

void setup() {
  Serial.begin(9600);
  // create banks of 1 x SIPO, all of same 3-wire interface and initialise
  // with each strobe pattern - 0b00000001, 0b00000010, 0b00000100, etc.
  // create_bank params are: data pin, clock pin, latch pin, number of SIPOs this bank
  for (bank = 0; bank < Num_SIPOs; bank++) {
    bank_ids[bank] = my_SIPOs.create_bank(8, 10, 9, 1);
    if (bank_ids[bank] == create_bank_failure) {
      Serial.println(F("failed to create bank"));
      Serial.flush();
      exit(0);
    }
    // now set up the strobe patterns in the bank's single SIPO, relative SIPO address is 0
    // sliding pattern of 1's starting at 0b00000001
    my_SIPOs.set_bank_SIPO(bank_ids[bank], 0, 1 << bank); // set up this bank's strobe pattern
  }
  // print the bank data and pin statuses for confirmation/inspection
  my_SIPOs.print_SIPO_data();
  my_SIPOs.print_pin_statuses();
}

void loop() {
  // scroll through every SIPO bank (interleave) and xfer the bank's pins
  // according to the direction for shift out.
  bool msb_or_lsb = MSBFIRST; // starting direction
  do {
    for (bank = 0; bank < Num_SIPOs; bank++) {
      my_SIPOs.xfer_bank(bank, msb_or_lsb); // xfer out this bank's SIPO pins
      delay(50);
    }
    msb_or_lsb = !msb_or_lsb; // switch direction
  } while (true);
}

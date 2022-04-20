//
//   Tutorial 4 - use of ez_SPI8 library,
//   2 x physical SIPOs, cascaded into a single SIPO bank
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

int bank_id;

#define Num_SIPOs   2
#define Num_timers  0

SIPO8 my_SIPOs(Num_SIPOs, Num_timers); // initiate the class for the tutorial

void setup() {
  Serial.begin(9600);
  // create a single bank of required number of SIPOs in the bank,
  // params are: data pin, clock pin, latch pin, number of SIPOs this bank
  bank_id = my_SIPOs.create_bank(8, 10, 9, 2);
  if (bank_id == create_bank_failure) {
    my_SIPOs.print_SIPO_data();
    Serial.println(F("failed to create bank"));
    Serial.flush();
    exit(0);
  }
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

// patterns to be xferred / shifted out to the SIPO bank
#define num_patterns 7
uint8_t patterns[num_patterns] = {// starting patterns
  0b11111111,
  0b00001111,
  0b00110011,
  0b01010101,
  0b11000011,
  0b01000010,
  0b00111100
};

void loop() {
  // scroll through every SIPO bank and assert the given pattern to every
  // SIPO defined by the bank.
  do {
    // setup each pattern in turn
    for (uint8_t pattern = 0; pattern < num_patterns; pattern++) {
      // perform 2 passes for each pattern - first, as defined, second inverted
      for (uint8_t cycle = 0; cycle < 2; cycle++) {
        // consider each SIPO in this 2 SIPO cascaded bank
        for (uint8_t sipo = 0; sipo < my_SIPOs.SIPO_banks[bank_id].bank_num_SIPOs; sipo++) {
          my_SIPOs.set_bank_SIPO(bank_id, sipo, patterns[pattern]); // set all pins of this SIPO in this bank_id
        }
        my_SIPOs.xfer_bank(bank_id, MSBFIRST);  // update physical SIPOs for this bank_id
        delay(500);
        patterns[pattern] = ~patterns[pattern]; // invert current pattern byte for next pass
      }
    }
  } while (true);
}

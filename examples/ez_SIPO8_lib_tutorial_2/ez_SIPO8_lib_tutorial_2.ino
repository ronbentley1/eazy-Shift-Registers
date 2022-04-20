//
//   Tutorial 2 - use of ez_SPI8 library,
//   Relative addressing - 1 x physical SIPO IC
//   The sketch demonstrates two ways in which SIPO output pins
//   may be updated, individually (set_bank_pin) or in a group
//   of 8 pins (set_bank_SIPO), representing a single 8it SIPO
//   within a bank.
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

#define Max_SIPOs  1  // two virtual SIPOs for this tutorial
#define Max_timers 0  // no timers required

// initiate the class for Max SIPOs/timers required
SIPO8 my_SIPOs(Max_SIPOs, Max_timers);

int bank_id;  // used to keep the SIPO bank id

void setup() {
  Serial.begin(9600);
  bank_id = my_SIPOs.create_bank(8, 10, 9, 1); // absolute pin addresses 0-7, relative addresses 0-7
  if (bank_id == create_bank_failure) {
    Serial.println(F("\nfailed to create bank"));
    Serial.flush();
    exit(0);
  }
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

void loop() {
  // start by setting the only SIPO (0) in the bank to all outputs off/LOW
  my_SIPOs.set_bank_SIPO(bank_id, 0, LOW);
  my_SIPOs.xfer_bank(bank_id, LSBFIRST);   // move virtual pin statuses to real SIPO output pins
  do {
    //
    // setup pattern for first cycle: 0b01010101
    // note that set_bank_pin uses relative addressing
    my_SIPOs.set_bank_pin(bank_id, 0, HIGH); // least significant bit/pin
    my_SIPOs.set_bank_pin(bank_id, 1, LOW);
    my_SIPOs.set_bank_pin(bank_id, 2, HIGH);
    my_SIPOs.set_bank_pin(bank_id, 3, LOW);
    my_SIPOs.set_bank_pin(bank_id, 4, HIGH);
    my_SIPOs.set_bank_pin(bank_id, 5, LOW);
    my_SIPOs.set_bank_pin(bank_id, 6, HIGH);
    my_SIPOs.set_bank_pin(bank_id, 7, LOW); // most significant bit/pin
    my_SIPOs.xfer_bank(bank_id, MSBFIRST);
    delay(500);
    //
    // setup reverse pattern using 8bit write function: 0b10101010
    // note that set_bank_SIPO uses relative addressing for SIPOs in the bank
    my_SIPOs.set_bank_SIPO(bank_id, 0, 0b10101010);
    my_SIPOs.xfer_bank(bank_id, MSBFIRST);
    delay(500);
  } while (true);
}

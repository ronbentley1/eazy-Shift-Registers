//
//   Strobe -
//   This sketch strobes a number of LEDs driven by a SIPO IC, 8 output pins
//   forwards and backwards at a defined frequency.
//
//   This example uses relative bank addressing.
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

#define Max_SIPOs  1 // 1 x SIPOs - provides 8 output pins
#define Max_timers 0 // no timers required, will use delay 

#define data_pin    8
#define clock_pin  10
#define latch_pin   9

#define strobe_frequency     50  // milli seconds, delay frequency

// initiate the class for max SIPOs/timers required
SIPO8 my_SIPOs(Max_SIPOs, Max_timers);

int bank_id;  // used to keep the SIPO bank id

void setup() {
  Serial.begin(9600);
  // create a bank of 'Max_SIPOs' using create_bank function:
  bank_id = my_SIPOs.create_bank(data_pin, clock_pin, latch_pin, Max_SIPOs);
  if (bank_id == create_bank_failure) {
    Serial.println(F("\nfailed to create bank, terminated"));
    Serial.flush();
    exit(0);
  }
  // start by setting all SIPO outputs to low (off) in the SIPO bank
  my_SIPOs.set_bank_SIPO(bank_id, 0, 0b00000000); // only 1 SIPO in bank,index 0
  my_SIPOs.xfer_bank(bank_id, MSBFIRST);
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

void loop() {
  //strobe example, strobe bank_id pins forward and back
  bool msb_or_lsb = MSBFIRST; // pick an initial strobe direction
  int pins_in_bank = my_SIPOs.num_pins_in_bank(bank_id); // number SIPO output pins in this bank
  if (pins_in_bank > 0) {
    do {
      for (uint16_t pin = 0; pin < pins_in_bank; pin++) {
        my_SIPOs.set_bank_pin(bank_id, pin, HIGH); // set next strobe pin 
        my_SIPOs.xfer_bank(bank_id, msb_or_lsb);   // update physical SIPO bank
        delay(strobe_frequency);
        my_SIPOs.set_bank_pin(bank_id, pin, LOW);  // clear next strobe pin 
        my_SIPOs.xfer_bank(bank_id, msb_or_lsb);   // update physical SIPO bank
      }
      msb_or_lsb = !msb_or_lsb; // switch strobe direction
    } while (true);
  } else {
    Serial.println(F("\nbank not found, terminated"));
    Serial.flush();
    exit(0);
  }
}

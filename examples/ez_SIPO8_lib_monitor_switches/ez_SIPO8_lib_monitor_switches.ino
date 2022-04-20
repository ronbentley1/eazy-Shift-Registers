/*
   Ron D Bentley, Stafford, UK
   April 2021

   Example sketch 1 for Arduino Community story board
   The sketch reads a number of toggle switches which are in one of two
   states - on(HIGH) or off(LOW).
   Each switch is mapped to a SIPO bank pin as follows:
   switches 0-5 -> bank pins 0-5
                   bank pin 6 is not used
                   bank pin 7 is used as a sketch running indicator,
                              or 'heart beat'

   Note that the switches are 'read' periodically sampled by a dummy
   function using the SIPO8 timers feature. The code is non-blocking.

   This example and code is in the public domain and
   may be used without restriction and without warranty.

*/

#include <ez_SIPO8_lib.h>

#define Max_SIPOs        1
#define Max_timers       1
#define sample_time    500  // interval period between sampling sensors
#define num_switches     6  // number of sensors requiring testing/reading in each cycle

// initiate the class for max SIPOs/timers required
SIPO8 my_SIPOs(Max_SIPOs, Max_timers);

uint8_t bank_id;

// dummy read switch function - returns a random staus for given switch
// number, based on switch's current status
bool read_switch_status(uint8_t Switch) {
  randomSeed(analogRead(A0)*analogRead(A2)); // keep changing seed
  if (random(0, 4) == 0)return !my_SIPOs.read_bank_pin(bank_id, Switch);
  return my_SIPOs.read_bank_pin(bank_id, Switch);
}

void setup() {
  Serial.begin(9600);
  // create 1 bank of Max_SIPOs
  // params are data pin, clock pin anf latch pin, number SIPOs
  bank_id = my_SIPOs.create_bank(8, 10, 9, Max_SIPOs);
  if (bank_id == create_bank_failure) {
    Serial.println(F("failed to create bank"));
    Serial.flush();
    exit(0);
  }
  my_SIPOs.print_SIPO_data();  // report on global SIPO8 params
}
// Scan all defined switches and set their respective bank pin status, LOW/HIGH.
// Switches/bank pins layout:
// Switch associated bank pins run from 0 to num_switches-1, inclusive (5) and
// are used to indicate respective switch status,
// bank pin 6 is not used and bank pin 7 is used to provide
// a 'heart beat' to indicate that the sketch is running.
void loop() {
  my_SIPOs.set_all_array_pins(LOW); // ensure we start with clear array pool/bank
  my_SIPOs.xfer_array(MSBFIRST);
  my_SIPOs.SIPO8_start_timer(timer0); // start the sample timer
  do {
    if (my_SIPOs.SIPO8_timer_elapsed(timer0, sample_time) == elapsed) {
      my_SIPOs.SIPO8_start_timer(timer0); // reset/restart the timer
      // read each switch and set its virtual SIPO pin in the bank
      my_SIPOs.invert_bank_pin(bank_id, 7); // flash 'heart beat'
      for (uint8_t Switch = 0; Switch < num_switches; Switch ++) {
        my_SIPOs.set_bank_pin(bank_id, Switch, read_switch_status(Switch));
      }
      my_SIPOs.xfer_bank(bank_id, MSBFIRST); // update the physical SIPO pins
    }
  } while (true);
}

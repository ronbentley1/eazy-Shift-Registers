//
//   Tutorial 3 - use of ez_SIPO8 library,
//   1x physical SIPO, and use of SIPO8 timers to control SIPO outputs with time
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

#define Max_SIPOs  1  // one virtual SIPO for this tutorial
#define Max_timers 8  // 8 timers required to map a complete 8bit SIPO

// initiate the class for Max SIPOs/timers required
SIPO8 my_SIPOs(Max_SIPOs, Max_timers);

int bank0_id;  // used to keep the SIPO bank id
//
// setup pin/LED flash data
uint8_t timer;
uint32_t timer_intervals[Max_timers] = {
  300, 400, 500, 600, 700, 800, 900, 1000 // millisecond elapse timer values
};
uint8_t timer_pins[Max_timers] = {
  0, 1, 2, 3, 4, 5, 6, 7 // SIPO output pin absolute addresses
};

void setup() {
  Serial.begin(9600);
  // create bank, params are:
  // data pin, clock pin, latch pin, number of SIPOs this bank
  bank0_id = my_SIPOs.create_bank(8, 10, 9, 1); 
  if (bank0_id == create_bank_failure) {
    Serial.println(F("\nfailed to create bank"));
    Serial.flush();
    exit(0);
  }
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

void loop() {
  // start by setting all SIPO outputs to low (off)
  my_SIPOs.set_all_array_pins(LOW);// set all declared virtual output pins LOW/off
  my_SIPOs.xfer_array(LSBFIRST);   // move virtual pin statuses to real SIPO output pins
  //
  // start all timers
  for (timer = 0; timer < Max_timers; timer++) {
    my_SIPOs.SIPO8_start_timer(timer);
  }
  timer = 0; // start checking at first timer
  do {
    // check each timer for elapse and, if elapsed, invert the timer's output pin
    // and reset the timer
    if (my_SIPOs.SIPO8_timer_elapsed(timer, timer_intervals[timer]) == elapsed) {
      my_SIPOs.invert_array_pin(timer_pins[timer]); // invert the pin associated with this timer
      my_SIPOs.xfer_array(MSBFIRST);                // update physical SIPO outputs
      my_SIPOs.SIPO8_start_timer(timer);            // reset/restart the current timer
    }
    timer++;  // move on to next timer
    if (timer >= Max_timers) timer = 0;  // wrap around to beginning
  } while (true);
}

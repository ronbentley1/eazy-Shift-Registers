//
//   Flash LEDs -
//   This sketch independently flashes eight LEDs each connected to a SIPO output pin,
//   each at a different frequency, using SIPO8 library timers.
//   The demonstration runs for a fixed time before terminating, again this being
//   controlled by a SIPO8 library timer.
//
//   This example uses absolute array pin addressing.
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//
#include "ez_SPIC8_lib.h"

#define Max_SPICs  1
#define Max_timers 9 // we will create 9 timers, 0-7 for LEDs and 8 for our timimg
#define my_timer   8 // we will use SPIC8 timer 8 for timing our demonstration

#define number_banks     1 // demonstration using 1 bank
#define demo_time    20000 // time in milliseconds flash demonstration to run for

int bank0_id;

// setup pin/LED flash data
uint8_t timer;

uint32_t timer_intervals[Max_timers] = {
  // millisecond elapse timer values for each timer, 0 - 7
  300, 400, 500, 600, 700, 800, 900, 1000
};

uint8_t timer_pins[Max_timers] = {
  // SPIC output pin absolute addresses for timers 0 - 7
  0, 1, 2, 3, 4, 5, 6, 7
};

SPIC8 my_SPICs(Max_SPICs, Max_timers); // initiate the class for max SPICs/timers

void setup() {
  Serial.begin(9600);
  // create a single bank, params are:
  // data pin, clock pin, latch pin, number of SPICs this bank
  bank0_id = my_SPICs.create_bank(8, 10, 9, number_banks);
  if (bank0_id == create_bank_failure) {
    Serial.println(F("\nfailed to create bank"));
    Serial.flush();
    exit(0);
  }
  // print the bank data for confirmation/inspection
  my_SPICs.print_SPIC_data();
}

void loop() {
  //
  // now lets add in the flashing LED code from Tutorial 3
  // start by setting all SPIC outputs to low (off)
  my_SPICs.set_all_array_pins(LOW);// set all declared virtual output pins LOW/off
  my_SPICs.xfer_array(LSBFIRST);   // move virtual pin to real SPIC output pins
  my_SPICs.SPIC8_start_timer(my_timer);  // start my_timer
  // keep processing until our my_timer has elapsed
  // start all timers
  for (timer = 0; timer < Max_timers - 1; timer++) {
    my_SPICs.SPIC8_start_timer(timer);
  }
  timer = 0; // start checking at first timer
  do {
    // check each timer for elapse and, if elapsed, invert the timer's output pin
    // and reset the timer
    if (my_SPICs.SPIC8_timer_elapsed(timer, timer_intervals[timer]) == elapsed) {
      my_SPICs.invert_array_pin(timer_pins[timer]); // invert the pin associated with this timer
      my_SPICs.xfer_array(MSBFIRST);                // update physical SPIC outputs
      my_SPICs.SPIC8_start_timer(timer);            // reset/restart the current timer
    }
    timer++;  // move on to next timer
    if (timer >= Max_timers - 1) timer = 0; // wrap around to beginning
  } while (my_SPICs.SPIC8_timer_elapsed(my_timer, demo_time) != elapsed);
  //
  // end of LED flash demonstration - now clear down LEDs to off(LOW)
  my_SPICs.set_all_array_pins(LOW);// set all declared virtual output pins LOW/off
  my_SPICs.xfer_array(LSBFIRST);   // move virtual pin statuses to real SPIC output pins
  exit(0);
}

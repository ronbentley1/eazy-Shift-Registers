//
//   Chasing LEDs -
//   This sketch illuminates a defined sequential array of LEDs connected to
//   SIPO output pins, at a specified frequency (milliseconds).
//   At the end of the LED sequence, the LEDs are reset and the
//   cycle repeated.
//
//   The sketch can be configured for a SIPO array of any length, but this
//   example is configured to demonstrate the chasing LEDs of a clock dial.
//   In this instance the SIPO array length will be 64 output pins (8 x SIPOs)
//   but only the first 60 outputs will be used (0-59 seconds). The frequency
//   will be set to 1 second (1000 millisecs).
//
//   This example uses absolute addressing of the defined SIPO array.
//
//   Ron D Bentley, Stafford, UK
//   April 2021
//
//   This example and code is in the public domain and
//   may be used without restriction and without warranty.
//

#include <ez_SIPO8_lib.h>

#define Max_SIPOs  8 // 8 x SIPOs - provides 64 output pins
#define Max_timers 1 // used to count 1 second elapsing 'beats'

#define data_pin    8
#define clock_pin  10
#define latch_pin   9

#define chase_length  60   // chasing seconds on a clock
#define frequency     1000 // milli seconds - 1 second frequency

// initiate the class for max SIPOs/timers required
SIPO8 my_SIPOs(Max_SIPOs, Max_timers);

int my_LEDs;  // used to keep the SIPO bank id

void setup() {
  Serial.begin(9600);
  // create a bank of 'Max_SIPOs' using create_bank function:
  my_LEDs = my_SIPOs.create_bank(data_pin, clock_pin, latch_pin, Max_SIPOs);
  if (my_LEDs == create_bank_failure) {
    Serial.println(F("\nfailed to create bank"));
    Serial.flush();
    exit(0);
  }
  // start by setting all SIPO outputs to low (off)
  my_SIPOs.set_all_array_pins(LOW);
  my_SIPOs.xfer_array(MSBFIRST);
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

void loop() {
  uint8_t  next_pin = 0;
  my_SIPOs.SIPO8_start_timer(timer0); // kick off the timer
  do {
    if (my_SIPOs.SIPO8_timer_elapsed(timer0, frequency) == elapsed) {
      // 1 second time elapsed, so update next pin in the array
      my_SIPOs.SIPO8_start_timer(timer0); // restart 1 second count for next cycle
      if (next_pin == chase_length) { // wrap around
        my_SIPOs.set_all_array_pins(LOW); // clear all pins
        next_pin = 0;
      } else {
        my_SIPOs.set_array_pin(next_pin, HIGH); // set absolute next_pin pin status
        next_pin++;
      }
      my_SIPOs.xfer_array(MSBFIRST); // update physical SIPOs
    }
  } while (true);
}

//
//   7 Segment LED Matrix -
//   Sketch uses a single SIPO IC to map the 7 segment matrix and creates
//   a single bank comprising one SIPO IC.
//
//   This sketch drives a single 7 segment LED matrix, displaying digits
//   from 0 to hex F, in two repeating cycles:
//   1. cycle 1 - shifting out the most significant bit first (MSBFIRST), with each
//      character appended with the DP character ".", eg "3."
//   2. cycle 2 - shifting out the least significant bit first (LSBFIRST), without the DP
//      character appended.
//
//   The sketch offers two methods for updating a 7 Segment LED Matrix, choice
//   is a simple matter a preference/familiarity.
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

#define max_SIPOs  1  // one 1 SIPO for this example
#define max_timers 0  // no timers required

// initiate the class for max SIPOs/timers required
SIPO8 my_SIPOs(max_SIPOs, max_timers);

int my_matrix7;  // used to keep the SIPO bank id

#define num_digits   16
uint8_t LSB_matrix_chars[num_digits] = {
  252, 96, 218, 242, 102, 182, 190, 224, 254, 246, 238, 62, 156, 122, 158, 142
};
#define LSB_DP_char 0b00000001  // "." value for LSBFIRST shiftout

uint8_t MSB_matrix_chars[num_digits] = {
  63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 119, 124, 57, 94, 121, 113
};
#define MSB_DP_char 0b10000000  // "." value for MSBFIRST shiftout

void setup() {
  Serial.begin(9600);
  // create a bank of 1 SIPO using create_bank function:
  // data pin, clock pin, latch pin, number of SIPO this bank
  my_matrix7 = my_SIPOs.create_bank(8, 10, 9, 1); // data pin, clock pin, latch pin, number of SIPO this bank
  if (my_matrix7 == create_bank_failure) {
    Serial.println(F("\nfailed to create bank"));
    Serial.flush();
    exit(0);
  }
  // print the bank data for confirmation/inspection
  my_SIPOs.print_SIPO_data();
}

void loop() {
  // keep running through the digits 0 to hex F, as defined by the
  // bit patterns in the preset array MSB_/LSB_matrix_chars
  do {
    // cycle 1 - MSBFIRST, with appended DP character "."
    my_SIPOs.set_bank_SIPO(my_matrix7, 0, 0b00000000);  // reset all LEDs in the matrix to off/LOW
    my_SIPOs.xfer_bank(my_matrix7, MSBFIRST);
    delay(50);
    for (uint8_t digit = 0; digit < num_digits; digit++) {
      my_SIPOs.set_bank_SIPO(my_matrix7, 0, MSB_matrix_chars[digit] + MSB_DP_char); // append "."
      my_SIPOs.xfer_bank(my_matrix7, MSBFIRST);
      delay(500);
    }
    // cycle 2 - LSBFIRST, no appended DP character
    my_SIPOs.set_bank_SIPO(my_matrix7, 0, 0b00000000);  // reset all LEDs in the matrix to off/LOW
    my_SIPOs.xfer_bank(my_matrix7, LSBFIRST);
    delay(50);
    for (uint8_t digit = 0; digit < num_digits; digit++) {
      // if DP char "." required to be appended to a char, then add 'LSB_DP_char'
      // to the 'LSB_matrix_chars[digit]' parameter
      my_SIPOs.set_bank_SIPO(my_matrix7, 0, LSB_matrix_chars[digit]);
      my_SIPOs.xfer_bank(my_matrix7, LSBFIRST);
      delay(500);
    }
  } while (true);
}

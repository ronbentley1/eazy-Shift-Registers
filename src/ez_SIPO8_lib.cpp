/*
   Ron D Bentley, Stafford, UK
   April 2021
   SIPO8 v1-00

   Serial/Parallel IC (SIPO) library supporting banking of multiple SIPOs
   of same/different bit sizes.
   Supports maximum of up to 255 8bit SIPOs (2040 individual output pins)
   and up to 255 indivual timers.

   This example and code is in the public domain and
   may be used without restriction and without warranty.

*/


#include <Arduino.h>
#include <ez_SIPO8_lib.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// This function will be called when the class is initiated.
// The parameter is the maximum number of SIPOs that will be configured
// in the sketch.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SIPO8::SIPO8(uint8_t max_SIPO_ICs, uint8_t Max_timers ) {
  // Setup the SIPO_banks control data struture sized for the maximum number of
  // SIPO banks that could be defined
  SIPO_banks = (SIPO_control *) malloc(sizeof(SIPO_control) * max_SIPO_ICs);
  if (SIPO_banks == NULL) {
    SIPO_lib_exit(0);
  }
  // Determine how may pin_status_bytes of 'pins_per_SIPO' bit length are
  // needed to map the number of bank SIPOs defined
  _max_pins = max_SIPO_ICs * pins_per_SIPO;
  max_pins  = _max_pins;
  _num_pin_status_bytes = max_SIPO_ICs;
  num_pin_status_bytes  = _num_pin_status_bytes;
  pin_status_bytes =  (uint8_t *) malloc(sizeof(pin_status_bytes) * _num_pin_status_bytes); // 1 byte per 8-bit SIPO
  if (pin_status_bytes == NULL) {
    SIPO_lib_exit(1);
  }
  // clear down pin_status_bytes to LOW (0)
  for (uint8_t pin_status_byte = 0; pin_status_byte < _num_pin_status_bytes; pin_status_byte++) {
    pin_status_bytes[pin_status_byte] = 0;
  }
  // create timer struct(ure) of required size
  timers = (timer_control *) malloc(sizeof(timer_control) * Max_timers);
  if (timers == NULL) {
    SIPO_lib_exit(2);
  }
  _max_timers = Max_timers;
  max_timers  = Max_timers;
  // initialise other working variables, private and user accessible
  for (uint8_t timer = 0; timer < max_timers; timer++) {
    timers[timer].timer_status = not_active;
    timers[timer].start_time = 0; // elapsed time
  }
  _num_active_pins = 0; // no pins yet declared
  num_active_pins  = 0;
  _max_SIPOs = max_SIPO_ICs;
  max_SIPOs  = _max_SIPOs;
  _bank_SIPO_count = 0;
  bank_SIPO_count  = 0;
  _next_bank = 0;
  num_banks  = 0;
}

//
// General error reporting routine with termination
//
void SIPO8::SIPO_lib_exit(uint8_t reason) {
  Serial.begin(9600);
  switch  (reason) {
    case 0:
      Serial.println(F("Exit:out of memory for setup-SIPO banks"));
      break;
    case 1:
      Serial.println(F("Exit:out of memory for setup-status bytes"));
      break;
    case 2:
      Serial.println(F("Exit:out of memory for setup-timers"));
      break;
    default:
      Serial.println(F("Exit:unspecified"));
      break;
  }
  Serial.flush();
  exit(reason);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// The function will try to create a bank of SIPOs if possible.  The create process
// will fail if the more SIPOs for a bank are requested than remain unallocated.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int SIPO8::create_bank(uint8_t data_pin, uint8_t clock_pin, uint8_t latch_pin,
                       uint8_t num_SIPOs) {
  if (_bank_SIPO_count + num_SIPOs <= _max_SIPOs  && num_SIPOs > 0) {
    // still enough free SIPOs available to assign to a new bank
    pinMode(data_pin,  OUTPUT);
    digitalWrite(data_pin, LOW);
    pinMode(clock_pin, OUTPUT);
    digitalWrite(clock_pin, LOW);
    pinMode(latch_pin, OUTPUT);
    digitalWrite(latch_pin, LOW);
    SIPO_banks[_next_bank].bank_data_pin  = data_pin;
    SIPO_banks[_next_bank].bank_clock_pin = clock_pin;
    SIPO_banks[_next_bank].bank_latch_pin = latch_pin;
    SIPO_banks[_next_bank].bank_num_SIPOs = num_SIPOs;
    SIPO_banks[_next_bank].bank_low_pin   = _num_active_pins;
    uint16_t num_pins_this_bank = num_SIPOs * pins_per_SIPO;
    SIPO_banks[_next_bank].bank_high_pin  = _num_active_pins + num_pins_this_bank - 1;// inclusive pin numbers
    _num_active_pins = _num_active_pins + num_pins_this_bank;
    num_active_pins  = _num_active_pins;
    _bank_SIPO_count = _bank_SIPO_count + num_SIPOs;
    bank_SIPO_count  = _bank_SIPO_count;
    _next_bank++;             // next bank struct(ure) entry
    num_banks = _next_bank;   // user accessible number of banks defined
    return _next_bank - 1;    // return the bank number of this bank in the SIPOs struct(ure)
  }
  return create_bank_failure; // cannot provide number of SIPOs asked for, for this bank request
}

//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will set the entire array of pins to the given status value.  Note that
// this function operates on an entire array basis rather than bank by bank.
// The parameter pin_status should be HIGH or LOW
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::set_all_array_pins(bool pin_status) {
  uint8_t mask = pin_status * 255; // either 0 (all pins set low), or 255 (all pins set high)
  for (uint8_t pin_status_byte = 0; pin_status_byte < _num_pin_status_bytes; pin_status_byte++) {
    pin_status_bytes[pin_status_byte] = mask;
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will invert the status of each pin the the array.   Note that
// this function operates on an entire array basis rather than bank by bank.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::invert_all_array_pins() {
  for (uint8_t pin_status_byte = 0; pin_status_byte < _num_pin_status_bytes; pin_status_byte++) {
    pin_status_bytes[pin_status_byte] = ~pin_status_bytes[pin_status_byte];
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function sets the given pin (absolute pin reference) to the given status value.
// The success or otherwise of the process may be tested by the calling code using
// the return function value.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int SIPO8::set_array_pin(uint16_t pin, bool pin_status) {
  if (pin < _num_active_pins) {
    // pin is in the defined pin range
    uint8_t pin_status_byte = pin / pins_per_SIPO;
    uint8_t pin_bit = pin % pins_per_SIPO;
    bitWrite(pin_status_bytes[pin_status_byte], pin_bit, pin_status);
    return pin;
  }
  return pin_set_failure;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function inverts the existing pin status. Note that pin is given as an absolute
// pin reference.
// The success or otherwise of the process may be tested by the calling code using
// the return function value.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int SIPO8::invert_array_pin(uint16_t pin) {
  if (pin < _num_active_pins) {
    // pin is in the defined pin range
    uint8_t pin_status_byte = pin / pins_per_SIPO;
    uint8_t pin_bit = pin % pins_per_SIPO;
    bool inverted_status = !bitRead(pin_status_bytes[pin_status_byte], pin_bit);
    bitWrite(pin_status_bytes[pin_status_byte],
             pin_bit,
             inverted_status);
    return inverted_status;  // high or low status
  }
  return pin_invert_failure;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Funcion will read the status value of the specified pin and return to the calling
// code. Note that pin is given as an absolute pin reference.
// The success or otherwise of the process may be tested by the calling code using
// the return function value.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int SIPO8::read_array_pin(uint16_t pin) {
  if (pin < _num_active_pins) {
    // pin is in the defined pin range
    uint8_t pin_status_byte = pin / pins_per_SIPO;
    uint8_t pin_bit = pin % pins_per_SIPO;
    return bitRead(pin_status_bytes[pin_status_byte], pin_bit);  // high or low status
  }
  return pin_read_failure;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function is equivalent to the set_all_array_pins function and is prvided as an
// alternative choice.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::set_banks(bool pin_status) {
  set_all_array_pins(pin_status);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will set every pin in each bank, from_bank - to_bank, to the
// specified pin status.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::set_banks(uint8_t from_bank, uint8_t to_bank, bool pin_status) {
  if (from_bank <= to_bank && to_bank < _next_bank) {
    for (uint8_t bank = from_bank; bank <= to_bank; bank++) {
      set_bank(bank, pin_status);
    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will set every pin in given bank to the specified pin status.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::set_bank(uint8_t bank, bool pin_status) {
  if (bank < _next_bank) {
    // start with the first pin of the first SIPO in the bank
    uint16_t first_pin = SIPO_banks[bank].bank_low_pin; // first pin in this bank
    uint8_t mask = pin_status * 255; // either 0 (all pins set low), or 255 (all pins set high)
    // determine the first pin status byte for the first pin in the bank
    uint8_t pin_status_byte = first_pin / pins_per_SIPO;
    // now deal with each SIPO declared in the bank, setting the
    // associated pin status bytes accordingly
    for (uint8_t SIPO = 0; SIPO < SIPO_banks[bank].bank_num_SIPOs; SIPO++) {
      pin_status_bytes[pin_status_byte + SIPO] = mask; // reset all pin bits
    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function is equivalent to the invert_all_array_pins function and is prvided as an
// alternative choice.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::invert_banks() {
  invert_all_array_pins();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will invert the existing pin status of every pin in the specified banks.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::invert_banks(uint8_t from_bank, uint8_t to_bank) {
  if (from_bank <= to_bank && to_bank < _next_bank) {
    for (uint8_t bank = from_bank; bank <= to_bank; bank++) {
      invert_bank(bank);
    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will invert the existing pin status of every pin in the specified bank.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::invert_bank(uint8_t bank) {
  if (bank < _next_bank) {
    // start with the first pin of the first SIPO in the bank
    uint16_t first_pin = SIPO_banks[bank].bank_low_pin; // first pin in this bank
    // determine the first pin status byte for the first pin in the bank
    uint8_t pin_status_byte = first_pin / pins_per_SIPO;
    // now deal with each SIPO declared in the bank, setting the
    // associated pin status bytes accordingly
    for (uint8_t SIPO = 0; SIPO < SIPO_banks[bank].bank_num_SIPOs; SIPO++) {
      pin_status_bytes[pin_status_byte + SIPO] =
        ~pin_status_bytes[pin_status_byte + SIPO]; // invert the status byte bits
    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function for setting specific pin within a given bank.
// Note that these functions operate relative to the pins defined
// in a bank - set_bank_pin, invert_bank_pin, read_bank_pin.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::set_bank_pin(uint8_t bank, uint8_t pin, bool pin_status) {
  if (bank < _next_bank) {
    pin = pin + SIPO_banks[bank].bank_low_pin;  // absolute pin number in the array
    return set_array_pin(pin, pin_status);      // returns failure or the absolute pin muber if successful
  }
  return pin_set_failure;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function for inverting specific existing pin status within a given bank.
// Note that these functions operate relative to the pins defined
// in a bank - set_bank_pin, invert_bank_pin, read_bank_pin.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::invert_bank_pin(uint8_t bank, uint8_t pin) {
  if (bank < _next_bank) {
    pin = pin + SIPO_banks[bank].bank_low_pin;  // absolute pin number in the array
    return invert_array_pin(pin);      // returns failure or the new status of the pin if successful
  }
  return pin_invert_failure;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will read the given pin's status and return its value to the calling code.
// Note that these functions operate relative to the pins defined
// in a bank - set_bank_pin, invert_bank_pin, read_bank_pin.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::read_bank_pin(uint8_t bank, uint8_t pin) {
  if (bank < _next_bank) {
    pin = pin + SIPO_banks[bank].bank_low_pin;  // absolute pin number in the array
    return read_array_pin(pin);     // returns failure or the pin status if successful
  }
  return pin_read_failure;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will set the given bank SIPO (8bits) to the secified value
// Note that this functions operate relative to the SIPOs/pins defined
// in a bank.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::set_bank_SIPO(uint8_t bank, uint8_t SIPO_num, uint8_t SIPO_value) {
  if (bank < _next_bank) {
    // bank is valid
    uint8_t SIPOs_this_bank = SIPO_banks[bank].bank_num_SIPOs;
    if (SIPO_num < SIPOs_this_bank) {
      // specified SIPO number is valid for this bank
      // now determine the pin_staus_byte entry for the SIPO
      uint8_t status_byte = SIPO_banks[bank].bank_low_pin / pins_per_SIPO;// first status byte for this bank
      status_byte = status_byte + SIPO_num; // actual status byte to be set
      pin_status_bytes[status_byte] = SIPO_value;// set required status byte
      return status_byte;
    }
    return SIPO_not_found;
  }
  return bank_not_found;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will invert the current contents of the given bank SIPO (8bits) to the
// secified value.
// Note that this functions operate relative to the SIPOs/pins defined
// in a bank.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::invert_bank_SIPO(uint8_t bank, uint8_t SIPO_num) {
  if (bank < _next_bank) {
    // bank is valid
    uint8_t SIPOs_this_bank = SIPO_banks[bank].bank_num_SIPOs;
    if (SIPO_num < SIPOs_this_bank) {
      // specified SIPO number is valid for this bank
      // now determine the pin_staus_byte entry for the SIPO
      uint8_t status_byte = SIPO_banks[bank].bank_low_pin / pins_per_SIPO;// first status byte for this bank
      status_byte = status_byte + SIPO_num; // actual status byte to be inverted
      pin_status_bytes[status_byte] = ~pin_status_bytes[status_byte]; // invert current contents
      return status_byte;
    }
    return SIPO_not_found;
  }
  return bank_not_found;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function will read the current contents of the given bank SIPO (8bits) and return
// the 8bit status value for the SIPO.
// Note that this functions operate relative to the SIPOs/pins defined
// in a bank.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::read_bank_SIPO(uint8_t bank, uint8_t SIPO_num) {
  if (bank < _next_bank) {
    // bank is valid
    uint8_t SIPOs_this_bank = SIPO_banks[bank].bank_num_SIPOs;
    if (SIPO_num < SIPOs_this_bank) {
      // specified SIPO number is valid for this bank
      // now determine the pin_staus_byte entry for the SIPO
      uint8_t status_byte = SIPO_banks[bank].bank_low_pin / pins_per_SIPO;// first status byte for this bank
      status_byte = status_byte + SIPO_num; // actual status byte to be read
      return pin_status_bytes[status_byte];
    }
    return SIPO_not_found;
  }
  return bank_not_found;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Given an absolute array pin number, the function determines which
// bank it resides within and returns the bank number.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int  SIPO8::get_bank_from_pin(uint16_t pin) {
  if (pin < _num_active_pins) {
    for (uint8_t bank = 0; bank < _next_bank; bank++) {
      if (SIPO_banks[bank].bank_low_pin <= pin &&
          pin <= SIPO_banks[bank].bank_high_pin)return bank;
    }
  }
  return bank_not_found;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Determines the number of pins in the given bank
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int SIPO8::num_pins_in_bank(uint8_t bank) {
  if (bank < _next_bank) {
    // valid bank, so return number of pins in this bank
    return SIPO_banks[bank].bank_num_SIPOs * pins_per_SIPO;
  }
  return bank_not_found;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Selective transfer of array pin satuses based on bank transfers,
// rather than the entire array of banks.
// Transfers specified banks' pin statuses to the hardware SIPOs,
// starting with from_bank and continuing to to_bank.
// The direction of transfer is determined by the msb_or_lsb parameter
// which must be either LSBFIRST  or MSBFIRST.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::xfer_banks(uint8_t from_bank, uint8_t to_bank, bool msb_or_lsb) {
  if (from_bank <= to_bank && to_bank < _next_bank) {
    // examine each bank in turn and deal with as many SIPOs as
    // are configured in each bank
    for (uint8_t bank = from_bank; bank <= to_bank; bank++) {
      uint16_t low_pin  = SIPO_banks[bank].bank_low_pin;   // start pin number for this bank
      uint16_t high_pin = SIPO_banks[bank].bank_high_pin;  // end inclusive pin number for this bank
      uint8_t latch_pin = SIPO_banks[bank].bank_latch_pin;
      uint8_t clock_pin = SIPO_banks[bank].bank_clock_pin;
      uint8_t data_pin  = SIPO_banks[bank].bank_data_pin;
      uint8_t num_SIPOs_this_bank = SIPO_banks[bank].bank_num_SIPOs;
      uint8_t SIPO_first_status_byte = SIPO_banks[bank].bank_low_pin  / pins_per_SIPO;
      uint8_t SIPO_last_status_byte  = SIPO_banks[bank].bank_high_pin / pins_per_SIPO;
      uint8_t SIPO_status_byte = 0;
      digitalWrite(latch_pin, LOW);   //  tell IC data transfer to start
      for (uint8_t SIPO = 0; SIPO < num_SIPOs_this_bank; SIPO++) {
        if (msb_or_lsb == LSBFIRST) {
          SIPO_status_byte = SIPO_first_status_byte + SIPO;
        } else {
          SIPO_status_byte = SIPO_last_status_byte - SIPO;
        }
        shift_out_bank(data_pin, clock_pin, pin_status_bytes[SIPO_status_byte], msb_or_lsb);
      }
      digitalWrite(latch_pin, HIGH);   //  tell IC data transfer is finished
    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Equivalent to xfer_array, and transfers all array pin statuses to the
// the hardware SIPOs. Provided as an alternative method.
// The direction of transfer is determined by the msb_or_lsb parameter
// which must be either LSBFIRST  or MSBFIRST.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::xfer_banks(bool msb_or_lsb) {
  if (_next_bank > 0) {
    xfer_banks(0, _next_bank - 1, msb_or_lsb);
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Specific transfer of the given bank pin statuses to the bank's associated
// hardware SIPOs. The direction of transfer is determined by the msb_or_lsb
// parameter which must be either LSBFIRST  or MSBFIRST.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::xfer_bank(uint8_t bank, bool msb_or_lsb) {
  if (_next_bank > 0) {
    xfer_banks(bank, bank, msb_or_lsb);
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Transfers all array pin statuses to the hardware SIPOs.
// The direction of transfer is determined by the msb_or_lsb parameter
// which must be either LSBFIRST  or MSBFIRST.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::xfer_array(bool msb_or_lsb) {
  xfer_banks(msb_or_lsb);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Based on the standard Arduino shiftout function.
// Moves out the given set of pin statuses, status_bits, to the specified SIPO.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::shift_out_bank(uint8_t data_pin, uint8_t clock_pin, uint8_t status_bits, bool msb_or_lsb)
{ // Shuffle each bit of the val parameter (0 or 1) one bit left
  // until all bits written out.
  for (uint8_t  i = 0; i < pins_per_SIPO; i++)  {
    if (msb_or_lsb == LSBFIRST) {
      digitalWrite(data_pin, !!(status_bits & (1 << i)));
    }
    else
    {
      digitalWrite(data_pin, !!(status_bits & (1 << (7 - i))));
    }
    digitalWrite(clock_pin, HIGH);
    digitalWrite(clock_pin, LOW);
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function is provided to assist end user to provide debug data during development.
// Prints all active pin status bytes, in bit form, starting with pin 0 and
// progressing to the last active end pin defined.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::print_pin_statuses() {
  if (_next_bank > 0) {
    Serial.println(F("\nActive pin array, pin statuses:"));
    // there is at least 1 bank
    uint8_t SIPO_count = 0;
    for (uint8_t bank = 0; bank < _next_bank; bank++) {
      Serial.print(F("Bank "));
      if (bank < 10)Serial.print(F(" "));
      Serial.print(bank);
      Serial.print(F(": MS"));
      uint8_t start_byte = SIPO_count;
      uint8_t last_byte = start_byte + SIPO_banks[bank].bank_num_SIPOs - 1;
      SIPO_count = SIPO_count + SIPO_banks[bank].bank_num_SIPOs;
      for (int16_t next_byte = last_byte; next_byte >= start_byte; next_byte--) {
        uint8_t SIPO_status = pin_status_bytes[next_byte];
        for (int pos = 7; pos >= 0; pos--) {
          uint8_t pin_status = bitRead(SIPO_status, pos);
          Serial.print(pin_status);
        }
      }
      Serial.println(F("LS"));
    }
  } else {
    Serial.println(F("\nNo banks defined"));
  }
  Serial.flush();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function is provided to assist end user to display debug data during development.
// Prints all setup data as a confirmation that the end user has correctly configured
// all key data.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::print_SIPO_data() {
  Serial.println(F("\nSIPO global values:"));
  Serial.print(F("pins_per_SIPO   = "));
  Serial.println(pins_per_SIPO);
  Serial.print(F("max_SIPOs       = "));
  Serial.println(_max_SIPOs);
  Serial.print(F("bank_SIPO_count = "));
  Serial.println(_bank_SIPO_count);
  Serial.print(F("num_active_pins = "));
  Serial.println(_num_active_pins);
  Serial.print(F("num_pin_status_bytes = "));
  Serial.println(_num_pin_status_bytes);
  Serial.print(F("next_free bank = "));
  if (_bank_SIPO_count < _max_SIPOs) {
    Serial.print(_next_bank);
    Serial.println();
  } else {
    Serial.println(F(" all SIPOs used"));
  }
  Serial.print("Number timers  =  ");
  Serial.println(_max_timers);
  Serial.println(F("\nBank data:"));
  for (uint8_t bank = 0; bank < _next_bank; bank++) {
    // still SIPOs available to assign to a new bank
    Serial.print(F("bank = "));
    Serial.println(bank);
    Serial.print(F("  num SIPOs =\t"));
    Serial.println(SIPO_banks[bank].bank_num_SIPOs);
    Serial.print(F("  latch_pin =\t"));
    Serial.print(SIPO_banks[bank].bank_latch_pin);
    Serial.print(F("  clock_pin =\t"));
    Serial.print(SIPO_banks[bank].bank_clock_pin);
    Serial.print(F("  data_pin  =\t"));
    Serial.println(SIPO_banks[bank].bank_data_pin);
    Serial.print(F("  low_pin   =\t"));
    Serial.print(SIPO_banks[bank].bank_low_pin);
    Serial.print(F("  high_pin  =\t"));
    Serial.println(SIPO_banks[bank].bank_high_pin);
  }
  Serial.flush();
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function sets the given timer as active and records the start time.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void SIPO8::SIPO8_start_timer(uint8_t timer) {
  if (timer < _max_timers) {
    timers[timer].timer_status = active;
    timers[timer].start_time = millis();
  }
}

//
// Function cancels the given timer.
//
void SIPO8::SIPO8_stop_timer(uint8_t timer) {
  if (timer < _max_timers) {
    timers[timer].timer_status = not_active;
  }
}

//
// Function determines if the time has elapsed for the given timer, if active.
//
bool SIPO8::SIPO8_timer_elapsed(uint8_t timer, uint32_t elapsed_time) {
  if (timer < _max_timers) {
    if (timers[timer].timer_status == active) {
      if (millis() - timers[timer].start_time >= elapsed_time) {
        timers[timer].timer_status = not_active; // mark this timer no longer active
        return elapsed;
      }
    }
  }
  return not_elapsed;
}
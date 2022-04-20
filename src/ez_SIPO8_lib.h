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

#ifndef SIPO8_h
#define SIPO8_h

#include <Arduino.h>  // always include this lib otherwise wont compile!

class SIPO8
{
  public:


#define pins_per_SIPO        8 // global 'set in stone' macro for a base SIPO unit

    // failure macros...
#define create_bank_failure -1
#define pin_read_failure    -1
#define pin_invert_failure  -1
#define pin_set_failure     -1
#define bank_not_found      -1
#define SIPO_not_found      -2

    // timer macros...
#define timer0               0
#define timer1               1
#define timer2               2
#define timer3               3
#define timer4               4
#define timer5               5
#define timer6               6
#define timer7               7
#define elapsed            true
#define not_elapsed        !elapsed
#define active             true
#define not_active         !active

    uint16_t max_pins             = 0; // user accessible params
    uint16_t num_active_pins      = 0; // ...
    uint8_t  num_pin_status_bytes = 0; // ...
    uint8_t  num_banks            = 0; // ...
    uint8_t  max_SIPOs            = 0; // ...
    uint8_t  bank_SIPO_count      = 0; // ...
    uint8_t  max_timers           = 0; // ...

    struct SIPO_control {
      uint8_t  bank_data_pin;
      uint8_t  bank_clock_pin;
      uint8_t  bank_latch_pin;
      uint8_t  bank_num_SIPOs;
      uint16_t bank_low_pin;
      uint16_t bank_high_pin;
    }*SIPO_banks;

    uint8_t * pin_status_bytes;  // records current status of each pin

    // timer control struct(ure)
    struct timer_control {
      bool     timer_status;    // records status of a timer - active or not active
      uint32_t start_time;      // records the millis time when a timer is started
    } *timers;

    // ******* function declarations....

    SIPO8(uint8_t, uint8_t); // constructor function called when class is initiated

    int  create_bank(uint8_t, uint8_t, uint8_t, uint8_t);
    void set_all_array_pins(bool);
    void invert_all_array_pins();
    int  set_array_pin(uint16_t, bool);
    int  invert_array_pin(uint16_t);
    int  read_array_pin(uint16_t);

    void set_banks(uint8_t, uint8_t, bool);
    void set_banks(bool);
    void set_bank(uint8_t, bool);
    void invert_banks();
    void invert_banks(uint8_t, uint8_t);
    void invert_bank(uint8_t);

    int  set_bank_SIPO(uint8_t, uint8_t, uint8_t);
    int  invert_bank_SIPO(uint8_t, uint8_t);
    int  read_bank_SIPO(uint8_t, uint8_t);

    int  set_bank_pin(uint8_t, uint8_t, bool);
    int  invert_bank_pin(uint8_t, uint8_t);
    int  read_bank_pin(uint8_t, uint8_t);

    int  get_bank_from_pin(uint16_t);
    int  num_pins_in_bank(uint8_t);

    void xfer_banks(uint8_t, uint8_t, bool);
    void xfer_banks(bool);
    void xfer_bank(uint8_t, bool);
    void xfer_array(bool);

    void print_pin_statuses();
    void print_SIPO_data();

    void SIPO8_start_timer(uint8_t);
    void SIPO8_stop_timer(uint8_t);
    bool SIPO8_timer_elapsed(uint8_t, uint32_t);

    // ****** private declarations.....
  private:
    uint16_t _max_pins             = 0;
    uint16_t _num_active_pins      = 0;
    uint8_t  _num_pin_status_bytes = 0;
    uint8_t  _max_SIPOs            = 0;
    uint8_t  _bank_SIPO_count      = 0;
    uint8_t  _next_bank            = 0;
    uint8_t  _max_timers           = 0;

    void SIPO_lib_exit(uint8_t);
    void shift_out_bank(uint8_t, uint8_t, uint8_t, bool);



};

#endif
/* WHCS Control Module
   Master Pin-out file
*/
#ifndef CONTROL_MODULE_PINS_H
#define CONTROL_MODULE_PINS_H

#include <avr/io.h>

#if !defined(__AVR_ATmega328P__)
 #error "Board type unsupported / not recognized"
#endif

#define PIN_MODE_INPUT(pin_def) \
  (pin_def ## _DDR &= ~pin_def ## _MSK)
#define PIN_MODE_OUTPUT(pin_def) \
  (pin_def ## _DDR |= pin_def ## _MSK)

#define PIN_HIGH(pin_def) \
  (pin_def ## _PORT |= pin_def ## _MSK)
#define PIN_LOW(pin_def) \
  (pin_def ## _PORT &= ~pin_def ## _MSK)
#define PIN_TOGGLE(pin_def) \
  (pin_def ## _PORT ^= pin_def ## _MSK)

#define PIN_READ(pin_def) \
  ((pin_def ## _PIN & pin_def ## _MSK) >> pin_def ## _MSK)

#include "pins.def.h"

#endif // CONTROL_MODULE_PINS_H

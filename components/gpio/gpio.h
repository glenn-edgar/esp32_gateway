#ifndef _GPIO_H_
#define _GPIO_H_


#include "olimex_gateway_external_pins.h"


void gpio_config_output_pin( int pin );
void gpio_config_input_pin( int pin );
void gpio_set_value(int pin, int value );
int gpio_read_value(int pin);


#endif



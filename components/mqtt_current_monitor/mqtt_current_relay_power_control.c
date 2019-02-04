

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "gpio.h"
#include "mqtt_current_relay_power_control.h"

#define EQUIPMENT_PIN  19
#define IRRIGATION_PIN 18
/*
   explanation Relays are normally open
   When active the relay is off and the GPIO pin is pulled high
   When inactive the relay is on the GPIO pin is pulled low
*/


#define ACTIVE_STATE    0
#define INACTIVE_STATE     1

static float equipment_limit_value;
static float irrigation_limit_value;
static bool  irrigation_state;
static bool  equipment_state;

float mqtt_current_get_equipment_level(void)
{
    return equipment_limit_value;
}

float mqtt_current_get_irrigation_level(void)
{
    return irrigation_limit_value;
}


void mqtt_current_store_voltage_limits( float equipment_limit, float irrigation_limit)
{
   equipment_limit_value  = equipment_limit;
   irrigation_limit_value = irrigation_limit;
    
}

void mqtt_current_get_relay_states( bool *equipment_state_value, bool *irrigation_state_value)
{
   *equipment_state_value = equipment_state;
   *irrigation_state_value = irrigation_state;   
    
}


void mqtt_relay_power_initialize(void)
{
    
    gpio_config_output_pin( EQUIPMENT_PIN );
    gpio_config_output_pin( IRRIGATION_PIN );
    gpio_set_value(EQUIPMENT_PIN, ACTIVE_STATE );
    gpio_set_value(IRRIGATION_PIN, ACTIVE_STATE );  
    irrigation_state = true;
    equipment_state = true;
}

void mqtt_relay_set_equipment_active( void )
{
    
    equipment_state = true;
    gpio_set_value(EQUIPMENT_PIN, ACTIVE_STATE );
}
void mqtt_relay_set_equipment_inactive( void )
{
    
    equipment_state = false;
    gpio_set_value(EQUIPMENT_PIN, INACTIVE_STATE );
    
    
}

void mqtt_relay_set_irrigation_active( void )
{
    
    irrigation_state = true;
    gpio_set_value(IRRIGATION_PIN, ACTIVE_STATE );
    
}


void mqtt_relay_set_irrigation_inactive( void )
{
     
    irrigation_state = false;
    gpio_set_value(IRRIGATION_PIN, INACTIVE_STATE );
   
    
}

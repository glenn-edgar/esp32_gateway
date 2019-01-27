

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "gpio.h"
#include "mqtt_current_relay_power_control.h"

#define EQUIPMENT_PIN  14
#define IRRIGATION_PIN 12
#define OFF_STATE    0
#define ON_STATE     1

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
    gpio_set_value(EQUIPMENT_PIN, OFF_STATE );
    gpio_set_value(IRRIGATION_PIN, OFF_STATE );  
    irrigation_state = false;
    equipment_state = false;
}

void mqtt_relay_set_equipment_on( void )
{
    
    equipment_state = true;
    gpio_set_value(EQUIPMENT_PIN, ON_STATE );
}
void mqtt_relay_set_equipment_off( void )
{
    
    equipment_state = false;
    gpio_set_value(EQUIPMENT_PIN, OFF_STATE );
    
    
}

void mqtt_relay_set_irrigation_on( void )
{
    
    irrigation_state = true;
    gpio_set_value(IRRIGATION_PIN, ON_STATE );
    
}


void mqtt_relay_set_irrigation_off( void )
{
     
    irrigation_state = false;
    gpio_set_value(IRRIGATION_PIN, OFF_STATE );
   
    
}

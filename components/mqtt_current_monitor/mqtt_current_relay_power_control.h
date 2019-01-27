#ifndef _MQTT_CURRENT_RELAY_POWER_H_
#define _MQTT_CURRENT_RELAY_POWER_H_

float mqtt_current_get_equipment_level(void);

float mqtt_current_get_irrigation_level(void);

void mqtt_current_store_voltage_limits( float equipment_limit, float irrigation_limit);


void mqtt_current_get_relay_states( bool *equipment_state_value, bool *irrigation_state_value);



void mqtt_relay_power_initialize(void);


void mqtt_relay_set_equipment_on( void );

void mqtt_relay_set_equipment_off( void );


void mqtt_relay_set_irrigation_on( void );


void mqtt_relay_set_irrigation_off( void );



#endif
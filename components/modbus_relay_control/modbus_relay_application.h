#ifndef _MODBUS_RELAY_APPLICATION_H_
#define _MODBUS_RELAY_APPLICATION_H_

void modbus_relay_contact_update(void);
void modbus_relay_reload_irrigation_timer(void);
void modbus_relay_update_io(void);
void modbus_relay_disable_all();
void initialize_modbus_relay_application_task(uint32_t number,const uint32_t *output_pins);



#endif
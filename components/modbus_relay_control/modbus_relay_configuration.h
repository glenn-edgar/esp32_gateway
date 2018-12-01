#ifndef _MODBUS_CONFIGURATION_H_
#define _MODBUS_CONFIGURATION_H_

bool modbus_relay_read_file_configuration( uint32_t *baud_rate,
                                     uint32_t *modbus_address, 
                                     bool  *rtu_flag,
                                     uint32_t  *parity,
                                     uint32_t max_output,
                                     uint32_t *number_of_outputs,
                                     uint32_t *relay_output_pins);

#endif

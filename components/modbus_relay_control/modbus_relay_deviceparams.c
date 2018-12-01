/*=====================================================================================
 * Description:
 *   C file to define parameter storage instances
 *====================================================================================*/
#include <stdint.h>
#include "modbus_relay_deviceparams.h"

// Here are the user defined instances for device parameters packed by 1 byte
// These are keep the values that can be accessed from Modbus master
holding_reg_params_t modbus_relay_holding_reg_params = { 0 };

input_reg_params_t modbus_relay_input_reg_params = { 0 };

coil_reg_params_t modbus_relay_coil_reg_params = { 0 };

discrete_reg_params_t modbus_relay_discrete_reg_params = { 0 };


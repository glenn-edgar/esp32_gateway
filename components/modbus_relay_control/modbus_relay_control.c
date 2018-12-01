#include <stdio.h>
#include <esp_types.h>
#include "esp_event.h"
#include "sdkconfig.h"
#include "mbcontroller.h"       // for mbcontroller defines and api
#include "modbus_relay_deviceparams.h"   
#include "freertos/task.h"
#include "modbus_relay_configuration.h"
#include "modbus_relay_application.h"
#include "modbus_relay_control.h"
#define MAX_OUTPUT 8
#define MB_PORT_NUM     (2) 

extern holding_reg_params_t modbus_relay_holding_reg_params;

extern input_reg_params_t modbus_relay_input_reg_params ;

extern coil_reg_params_t modbus_relay_coil_reg_params;

extern discrete_reg_params_t modbus_relay_discrete_reg_params ;

static uint32_t modbus_address;
static uint32_t number_of_outputs;
static uint32_t relay_output_pins[ MAX_OUTPUT];
static uint32_t baud_rate;
static bool rtu_flag;
static uint32_t parity; 


static  mb_param_info_t reg_info; // keeps the Modbus registers access information
static  mb_communication_info_t comm_info; // Modbus communication parameters
static  mb_register_area_descriptor_t reg_area; 

static void modbus_relay_task( void *ptr);
static void initialize_modbus_hardware(void);


void initialize_modbus_relay_control(void)
{
    if( modbus_relay_read_file_configuration( &baud_rate,
                                        &modbus_address, 
                                        &rtu_flag,
                                        &parity,
                                        MAX_OUTPUT,
                                        &number_of_outputs,
                                        relay_output_pins) == false)
    {
        return;
    }
    initialize_modbus_hardware();
    initialize_modbus_relay_application_task(number_of_outputs, relay_output_pins);    
    xTaskCreate( modbus_relay_task, "MODBUS_RELAY_TASK",4000,
                  NULL, 20, NULL );
    
}


static void initialize_modbus_hardware(void)
{
    
 

    mbcontroller_init(); // Initialization of Modbus controller

    // Setup communication parameters and start stack
    if(rtu_flag == true)
    {    
        comm_info.mode = MB_MODE_RTU;
    }
    else
    {
       comm_info.mode = MB_MODE_ASCII;
    }
    comm_info.slave_addr = modbus_address;
    comm_info.port = MB_PORT_NUM;
    comm_info.baudrate = baud_rate;
    comm_info.parity = parity;
    printf("comm_info setup %d \n",mbcontroller_setup(comm_info));


    
    
    reg_area.type = MB_PARAM_HOLDING; // Set type of register area
    reg_area.start_offset = 0; // Offset of register area in Modbus protocol
    reg_area.address = (void*)&modbus_relay_holding_reg_params; // Set pointer to storage instance
    reg_area.size = sizeof(modbus_relay_holding_reg_params); // Set the size of register storage instance
    printf("msb set descriptor %d \n ", mbcontroller_set_descriptor(reg_area));

    // Initialization of Input Registers area
    reg_area.type = MB_PARAM_INPUT;
    reg_area.start_offset = 0;
    reg_area.address = (void*)&modbus_relay_input_reg_params;
    reg_area.size = sizeof(modbus_relay_input_reg_params);
    printf("mb set descriptor %d \n", mbcontroller_set_descriptor(reg_area));

    // Initialization of Coils register area
    reg_area.type = MB_PARAM_COIL;
    reg_area.start_offset = 0;
    reg_area.address = (void*)&modbus_relay_coil_reg_params;
    reg_area.size = sizeof(modbus_relay_coil_reg_params);
    printf("set descriptor %d \n",mbcontroller_set_descriptor(reg_area));

    // Initialization of Discrete Inputs register area
    reg_area.type = MB_PARAM_DISCRETE;
    reg_area.start_offset = 0;
    reg_area.address = (void*)&modbus_relay_discrete_reg_params;
    reg_area.size = sizeof(modbus_relay_discrete_reg_params);
    printf("set descriptor %d \n",mbcontroller_set_descriptor(reg_area));
    
    
}    
  
static void modbus_relay_task( void *ptr)
{
   mb_event_group_t event;
   while(1)
   {       
    
        // Check for read/write events of Modbus master for certain events
        event = mbcontroller_check_event((MB_EVENT_HOLDING_REG_WR |MB_EVENT_COILS_WR ));

        // Filter events and process them accordingly
        if(event & MB_EVENT_HOLDING_REG_WR)  
        {
            

            if (reg_info.address == (uint8_t*)&modbus_relay_holding_reg_params.wd_register)
            {
                modbus_relay_holding_reg_params.wd_register = 0;
                modbus_relay_contact_update();
            }
            if (reg_info.address == 
                (uint8_t*)&modbus_relay_holding_reg_params.irrigation_counter)
            {
                modbus_relay_reload_irrigation_timer();
            }
            if (reg_info.address == 
                (uint8_t*)&modbus_relay_holding_reg_params.disable_all)
            {
                modbus_relay_holding_reg_params.disable_all = 0;
                modbus_relay_disable_all();
            }
        } 
        if(event & MB_EVENT_HOLDING_REG_WR)
        {
            modbus_relay_update_io();
        }
       

    }
    // Destroy of Modbus controller once get maximum value of data_chan0
    printf("Modbus controller destroyed.");
    mbcontroller_destroy();
}
   
  
    
    
    

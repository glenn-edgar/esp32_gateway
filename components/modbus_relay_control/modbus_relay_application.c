
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "esp_event.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "cf_events.h"
#include "watchdog.h"
#include "app_input_functions.h"
#include "chain_flow_support.h"

#include "modbus_relay_deviceparams.h"
#include "modbus_relay_application.h"

extern holding_reg_params_t modbus_relay_holding_reg_params;
extern input_reg_params_t modbus_relay_input_reg_params;
extern coil_reg_params_t modbus_relay_coil_reg_params;
extern discrete_reg_params_t modbus_relay_discrete_reg_params;


#define CF_HOST_CONTACT CF_APP_EVENT_START +1
#define CF_RESTART_TIMER CF_HOST_CONTACT +1
#define CF_TERMINATE_TIMER CF_RESTART_TIMER +1
#include "chain_flow_assembler_a/cf_chain_flow_include.h"


static void modbus_relay_application_task(void *prt);
static uint32_t number_of_outputs ;
static const uint32_t *relay_output_pins;

static CHAIN_FLOW_HANDLE cf;


static void load_chain_flow_data( CHAIN_FLOW_HANDLE *cf );

static void  configure_pins_for_output(void)
{
    modbus_relay_coil_reg_params.data_field = 0xffffffff;
    for(int i = 0; i<number_of_outputs; i++)
    {
     gpio_pad_select_gpio(relay_output_pins[i]);
     gpio_set_direction(relay_output_pins[i], GPIO_MODE_OUTPUT);
     gpio_set_level(relay_output_pins[i],1);   
    }
    
    
}

void modbus_relay_contact_update(void)
{
    modbus_relay_holding_reg_params.wd_register =0;
    printf("host watchdog contact \n");
    cf_send_event( &cf,CF_HOST_CONTACT,1 );  
    
}

void modbus_relay_reload_irrigation_timer(void)
{
    cf_send_event( &cf,CF_RESTART_TIMER,1 );  
    printf("modbus_relay reload irrigation timer \n");
    
}

void modbus_relay_update_io(void)
{
   uint32_t mask;
   mask = 1;
   for(int i = 0; i< number_of_outputs;i++)
   {
       gpio_set_level(relay_output_pins[i],modbus_relay_coil_reg_params.data_field&mask);
       mask =mask +mask;
   }
 
    
}

void modbus_relay_disable_all(void)
{
    printf("disable all \n");
    cf_send_event( &cf,CF_TERMINATE_TIMER,1 );
    memset(&modbus_relay_coil_reg_params,0xff,sizeof(modbus_relay_coil_reg_params));
    modbus_relay_holding_reg_params.irrigation_counter = 0;
    modbus_relay_update_io();
}

void initialize_modbus_relay_application_task(uint32_t number, const uint32_t *output_pins)
{
    number_of_outputs = number;
    relay_output_pins = output_pins;
    configure_pins_for_output();
    load_chain_flow_data( &cf );
    initialize_cf_system(&cf);
    xTaskCreate( modbus_relay_application_task, "MODBUS_RELAY_APPLICATION_TASK",4000,
                  NULL, 20, NULL );
    
}


int modbus_relay_disable_irrigation_cf(CHAIN_FLOW_HANDLE *cf, 
                                           unsigned link_id, 
                                           unsigned param_1,
                                           unsigned param_2, 
                                           unsigned param_3, 
                                           unsigned event, 
                                           unsigned data)
{
      
   modbus_relay_disable_all();
   return CF_DISABLE;   
      
}  

int modbus_relay_check_irrigation_timer_cf(CHAIN_FLOW_HANDLE *cf, 
                                       unsigned link_id, unsigned param_1,
                                       unsigned param_2, 
                                       unsigned param_3, 
                                       unsigned event, 
                                       unsigned data)
{
    
    if( event != CF_SECOND_TICK)
    {
       return 0; 
    }
    if(modbus_relay_holding_reg_params.irrigation_counter == 0 )
    {
        return 1;
    }
    modbus_relay_holding_reg_params.irrigation_counter -= 1;
    
    if(modbus_relay_holding_reg_params.irrigation_counter == 0 )
    {
        return 1;
    }
    return 0;    
    
}                                      



static void modbus_relay_application_task(void *prt)
{
    int second_sub_count;
    int minute_sub_count;
    
    uint32_t event_number;
    unsigned event_id;
    unsigned event_data;
    
    
    minute_sub_count = 0;
    second_sub_count = 0;
    
    
    
    
  

    for(;;)
    {
        while( cf_event_number(&cf) != 0)
        {
           
           
             event_number = cf_rx_event( &cf, &event_id, &event_data );
             if( event_number > 0 )
             {
              
              cf_process_cf_event( &cf,event_id, event_data);
             }
           
        }
        vTaskDelay(10/ portTICK_PERIOD_MS); 
        cf_send_event( &cf,CF_TIME_TICK_EVENT, 10 );
        second_sub_count += 10;
        if(second_sub_count >= 1000)
        {
          second_sub_count = 0;
          
          cf_send_event( &cf,CF_SECOND_TICK, 1 );
          minute_sub_count += 1;
          if(minute_sub_count >= 60 )
          {
              cf_send_event( &cf,CF_MINUTE_TICK,1 );
              
              minute_sub_count = 0;
          }
            
        }
 
       
    }       
      
    
}

static void load_chain_flow_data( CHAIN_FLOW_HANDLE *cf )
{
    memset(cf,0,sizeof(CHAIN_FLOW_HANDLE));
 
    cf->number_of_chains =CHAIN_NUM;
    cf->number_of_links = LINK_NUM;
    cf->chain_state = chain_state;
    cf->link_state = link_state;
    cf->link_data  = link_data;
    cf->start_state = start_state;
    cf->chain_control = chain_control;

}

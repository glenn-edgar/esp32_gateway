

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "esp_event.h"
#include "freertos/task.h"
#include "cf_events.h"
#include "cf_status_bit.h"
#include "mqtt_current_functions.h"
#include "chain_flow_support.h"
#include "chain_flow_assembler/cf_chain_flow_include.h"
#include "mqtt_current_relay_power_control.h"
#include "mqtt_current_read_config.h"
#include "mqtt_current_main.h"

static TaskHandle_t xHandle = NULL;
static void mqtt_current_task( void * pvParameters );
static void load_chain_flow_data( CHAIN_FLOW_HANDLE *cf );



void initialize_mqtt_current_main(void)
{
       float max_current_equipment;
       float max_current_irrigation;
       if( mqtt_current_file_read_configuration(&max_current_equipment, &max_current_irrigation) == true)
       {
          mqtt_current_store_voltage_limits( max_current_equipment, max_current_irrigation);
          xTaskCreate( mqtt_current_task, "MQTT_CURRENT_TASK",12000, NULL, 20, &xHandle );
          configASSERT( xHandle );
       }
 
    
}
static inline void  process_status_data( CHAIN_FLOW_HANDLE *cf, unsigned status_data)
{

;
    
}

// Task to be created.
static void mqtt_current_task( void * pvParameters )
{
    int second_sub_count;
    int minute_sub_count;
    uint32_t status_data;
    uint32_t event_number;
    unsigned event_id;
    unsigned event_data;
    CHAIN_FLOW_HANDLE cf;
    
    minute_sub_count = 0;
    second_sub_count = 0;
    mqtt_relay_power_initialize();

    load_chain_flow_data( &cf );
    
    initialize_cf_system(&cf);
    
    for(;;)
    {
        while( cf_event_number(&cf) != 0)
        {
           status_data = cf_get_status(&cf);
           if( status_data != 0 )
           {
               process_status_data(&cf, status_data);
             
           }
           else
           {
           
             event_number = cf_rx_event( &cf, &event_id, &event_data );
             if( event_number > 0 )
             {
              
              cf_process_cf_event( &cf,event_id, event_data);
             }
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
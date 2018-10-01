

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "esp_event.h"
#include "freertos/task.h"
#include "cf_events.h"
#include "cf_status_bit.h"
#include "app_input_functions.h"
#include "cf_chain_flow_support.h"

#include "app_input_main.h"

static TaskHandle_t xHandle = NULL;
static void app_input_task( void * pvParameters );


void initialize_app_input_main(void)
{
   if(app_input_input_data_read()==true)
   {
       xTaskCreate( app_input_task, "APP_INPUT_TASK",4000, NULL, 20, &xHandle );
       configASSERT( xHandle );
   }
    
}
static inline void  process_status_data( unsigned status_data)
{

#if 0   
    if( status_data &  CF_PROCESS_TIMER_TICK )
    {
        cf_process_cf_event( CF_TIME_TICK_EVENT, TICK_INTERVAL );
        cf_status_clear_bit(  CF_PROCESS_TIMER_TICK );
    }
    if( status_data & CF_PROCESS_MODBUS )
    {
      pat_watch_dog(0,0,0,0,0,0);  // parameters for pat_watch_dog is not used
      process_rx_modbus_rtu_message( );
      cf_status_clear_bit(  CF_PROCESS_MODBUS );
    }
#endif
    
}

// Task to be created.
static void app_input_task( void * pvParameters )
{
    int second_sub_count;
    int minute_sub_count;
    uint32_t status_data;
    uint32_t event_number;
    unsigned event_id;
    unsigned event_data;
    
    minute_sub_count = 0;
    second_sub_count = 0;
    
    
   
    
    initialize_cf_system( );

    for(;;)
    {
        while( cf_event_number() != 0)
        {
           status_data = cf_get_status();
           if( status_data != 0 )
           {
               process_status_data( status_data);
             
           }
           else
           {
           
             event_number = cf_rx_event( &event_id, &event_data );
             if( event_number > 0 )
             {
              
              cf_process_cf_event( event_id, event_data);
             }
           }
        }
        vTaskDelay(10/ portTICK_PERIOD_MS); 
        cf_send_event( CF_TIME_TICK_EVENT, 10 );
        second_sub_count += 10;
        if(second_sub_count >= 1000)
        {
          second_sub_count = 0;
          
          cf_send_event( CF_SECOND_TICK, 1 );
          minute_sub_count += 1;
          if(minute_sub_count >= 60 )
          {
              cf_send_event( CF_MINUTE_TICK,1 );
              printf("sending minute event \n");
              minute_sub_count = 0;
          }
            
        }
 
       
    }       
      
    
}
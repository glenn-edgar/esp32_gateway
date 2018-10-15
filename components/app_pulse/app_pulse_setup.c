#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "driver/pcnt.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "driver/mcpwm.h"
#include "msgpack_rx_handler.h"
#include "app_pulse_setup.h"


typedef struct
{
    uint32_t gpio;
    uint32_t pulse_filter;
    
}APP_PULSE_TIMER_CONTROL;

static uint32_t pulse_update_rate;
static uint32_t pulse_counter_number;
static APP_PULSE_TIMER_CONTROL pulse_counters[PCNT_UNIT_MAX] ;

static bool app_pulse_read_file_configuration( void );  
static bool app_pulse_find_set_configuration(   uint32_t size, char *buffer);
static void setup_pulse_counters( uint32_t number_of_counters);
static void setup_a_pulse_counter( uint32_t counter, uint32_t gpio_pin,uint32_t min_pulse_length );
static int32_t read_a_counter( uint32_t counter);

static void counter_task(void *arg);
                             
void initialize_app_pulse_main(void)
{

    pulse_counter_number = 0;
    memset(pulse_counters,0,sizeof(pulse_counters));
    
    if( app_pulse_read_file_configuration( ) == true)
    {

       
       xTaskCreate(counter_task, "counter_task", 4000, NULL, 40, NULL);
    }

        
}
    

                              






static void counter_task(void *arg)
{
    uint32_t pulse_count;
    setup_pulse_counters( pulse_counter_number);
    while(1)
    {
       vTaskDelay(1000/ portTICK_PERIOD_MS); 
       // read counters and accumulate
       if(pulse_count >= pulse_update_rate) 
       {
           // send message
           pulse_count = 0;
           // clear pulse accumulators
       }
           
    
    }
    
    
    
    
    
}


 
/*
** PULSE HARDWARE STUFF
**
**
*/ 

static void setup_pulse_counters( uint32_t number_of_counters)
{
    for(int i = 0; i< number_of_counters;i++)
    {
        //setup_a_pulse_counter(i);
        
        
    }
    
    
    
}



static void setup_a_pulse_counter( uint32_t counter, uint32_t gpio_pin, uint32_t min_pulse_length )
{
    pcnt_config_t pcnt_config;
    pcnt_config.pulse_gpio_num = gpio_pin;
    pcnt_config.ctrl_gpio_num = -1;
    pcnt_config.unit = counter;
    pcnt_config.pos_mode = PCNT_COUNT_INC;   // Count up on the positive edge
    pcnt_config.neg_mode = PCNT_COUNT_INC;   // Keep the counter value on the negative edge
    pcnt_config.counter_h_lim = 0x7fff;
    pcnt_config.counter_l_lim = 0;


    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);
    pcnt_set_filter_value(counter, min_pulse_length);
    pcnt_counter_clear(counter);
    //counter start


}

static int32_t read_a_counter( uint32_t counter)
{
    int16_t count;
    
    pcnt_counter_pause(counter);
    pcnt_get_counter_value(counter, &count);
    pcnt_counter_clear(counter);
    // counter start
    return count;
    
}





/*
**  MESSAGE PACK STUFF
**
*/
static bool app_pulse_read_file_configuration( void ) 
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
       
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_PULSE.MPK" ,  &buffer,&buffer_size ) != true)
    {
        
         return false;
    }
   
    
    return_value = app_pulse_find_set_configuration(   buffer_size, buffer);
    
    free(buffer);
    return return_value;    
    
}

static bool app_pulse_find_set_configuration(   uint32_t size, char *buffer)
{
    
    return false;
}
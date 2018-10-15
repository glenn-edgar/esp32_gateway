
#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "app_analog_dac.h"
#include "app_analog_task.h"
#include "app_analog_mqtt_update.h"
#include "app_analog_analog_configuration.h"
#include "app_analog_event_queue.h"
#include "app_analog_task.h"



static xQueueHandle timer_queue;
static xQueueHandle mqtt_queue;


static void analog_sampling_task(void *arg);

void app_analog_initialize_sampling_task( xQueueHandle tm_que, xQueueHandle mqtt_que )
{
    timer_queue = tm_que;
    mqtt_queue = mqtt_que;
    // setup gpio
    xTaskCreate(analog_sampling_task, "analog_task", 4000, NULL, 40, NULL);
    
}







static void analog_sampling_task(void *arg)
{
    
    uint32_t ad_buffer_count;
 #if 0   
    ad_buffer_count = 0;
    // setup interrupt timer
    while (1) 
    {
        a_d_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
        for(int i = 0; i<analog_channels; i++)
        {
          // compute dc fft
          // compute ac waveform
          // compute ac fft
        
               
            
        }
        ad_buffer_count++;
        if( ad_buffer_count = mqtt_update_time)
        {
            
           xQueueSend(mqtt_queue, &evt, NULL); 
        }

 

    }
#endif
}
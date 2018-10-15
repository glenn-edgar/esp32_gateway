#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "esp_event.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "msg_dict_stream.h"
#include "app_analog_event_queue.h"
#include "app_analog_mqtt_update.h"

#define MESSAGE_PACK_NUMBER 2+4*ANALOG_CHANNEL_NUMBER

static APP_MQTT_STRUCTURE mqtt_structure;
static MSG_PACK_ELEMENT msg_pack[ANALOG_CHANNEL_NUMBER];

static xQueueHandle mqtt_queue;

static void mqtt_packaging_queue(void *arg);

void app_analog_initialize_mqtt_task(  xQueueHandle mqtt_que )
{
    
    mqtt_queue = mqtt_que;
    xTaskCreate(mqtt_packaging_queue, "analog_mqtt_task", 4000, NULL, 10, NULL);
    
}




static void mqtt_packaging_queue(void *arg)
{
    int buffer_size;
    char *buffer;

    APP_ANALOG_MQTT_CHANNEL *analog_mqtt;
    while (1) 
    {
     
        xQueueReceive(mqtt_queue, &mqtt_structure, portMAX_DELAY);
        msg_dict_pack_string(&msg_pack[0],"TOPIC","INPUT/GPIO/CHANGE");      
        msg_dict_pack_array(&msg_pack[1],"MEASUREMENTS", mqtt_structure.channel_number);
        for(int i = 0; i<mqtt_structure.channel_number; i++)
        {
            analog_mqtt  = &mqtt_structure.app_analog_mqtt_channel[i];
            msg_dict_pack_map(&msg_pack[4*i+2],"DATA", 3);
            msg_dict_pack_unsigned_integer(&msg_pack[4*i+3],"GPIO", analog_mqtt->gpio_pin);
            msg_dict_pack_float(&msg_pack[4*i+4], "DC",analog_mqtt->dc_average);
            msg_dict_pack_float(&msg_pack[4*i+5],"AC",analog_mqtt->ac_average);
           
            
        }
        buffer =  msg_dict_stream( &buffer_size, mqtt_structure.channel_number*4+2, msg_pack);
        mqtt_clt_publish("INPuT/ANALOG/VALUE", buffer,buffer_size );
        free(buffer);
  
    }
}
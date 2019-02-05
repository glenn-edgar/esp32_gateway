
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>

#include "msgpack_rx_handler.h"
#include "mqtt_current_read_config.h"

static bool mqtt_current_read_setup( float *max_current_equipment, float *max_current_irrigation,uint32_t data_len ,char *data );



bool mqtt_current_file_read_configuration( float *max_current_equipment, float *max_current_irrigation )
{
    
    
   bool return_value;
    char     *buffer;
    uint32_t buffer_size = 1000;
    cmp_ctx_t ctx;

    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/MQTT_CUR.MPK",  &buffer,&buffer_size ) != true)
    {
         printf("############## cannot read file /spiffs/MQTT_CUR.MPK \n");
         return false;
    }
   
    return_value = mqtt_current_read_setup( max_current_equipment,max_current_irrigation,buffer_size, buffer );
    free(buffer);
    
    return return_value;    
    
}


static bool mqtt_current_read_setup( float *max_current_equipment, float *max_current_irrigation,uint32_t data_len ,char *data )
{
 
    cmp_ctx_t ctx;
   
    msgpack_rx_handler_init(&ctx, data, data_len);
    if( msgpack_rx_handler_find_float(&ctx,"max_current_equipment",max_current_equipment )!= true)
    {
         goto exit;
    }
    printf("max current equipment %f \n",*max_current_equipment);
   if( msgpack_rx_handler_find_float(&ctx,"max_current_irrigation", max_current_irrigation )!= true)
    {
        goto exit;
    }
    printf("max_current_irrigation %f \n",*max_current_irrigation);
    return true;


exit:
   
    return false;    
    
}


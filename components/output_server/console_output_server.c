
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include "freertos/semphr.h"

#include "console_output_server.h"
#include "base64.h"
#include "console_output_server.h"
#include "hexadecimal.h"

SemaphoreHandle_t xSemaphore = NULL;

bool console_output_server(void)
{
     vSemaphoreCreateBinary( xSemaphore );
     return true;
    
}

bool console_output(const char *format, ...)
{
    bool return_value;
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
        va_list args;
        va_start(args, format);
        return_value = vprintf(format, args);
        va_end(args);
        xSemaphoreGive( xSemaphore );
    }
    else
    {
        return_value = false;
    }
    return return_value;
    
}


bool console_output_structured_data(int number, MSG_PACK_ELEMENT *msg_pack)
{
    bool return_value;
    char *pack_buffer;
    int   pack_buffer_size;
    char *master_buffer;
    //char *current_pointer;
    
    pack_buffer = msg_dict_stream( &pack_buffer_size,number,msg_pack);
    master_buffer = malloc(pack_buffer_size*2+50);
    memset(master_buffer,0,pack_buffer_size*2+50);
    strcpy(master_buffer,"BASE64:");
    printf("pack buffer size %d \n",pack_buffer_size);
    binary_to_hex(pack_buffer,pack_buffer_size, master_buffer+strlen("BASE64:"));
    strcat(master_buffer,":END");
    
 
 
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
        
         puts(master_buffer);
         xSemaphoreGive( xSemaphore );
    }
    else
    {
        return_value = false;
    }
    free(pack_buffer);
    free(master_buffer);
    
    return return_value;
       
    
    
}


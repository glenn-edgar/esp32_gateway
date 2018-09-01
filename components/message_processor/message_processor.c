
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include "freertos/semphr.h"



#include "hexadecimal.h"
#include "message_processor.h"

static SemaphoreHandle_t xSemaphore = NULL;
static void process_message(char * input_buffer,int input_number);


void init_message_processor(void)
{
    vSemaphoreCreateBinary( xSemaphore );
    
}

void message_processor(char * input_buffer,int input_number)
{
    
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == true )
    {
        process_message(input_buffer,input_number);
        xSemaphoreGive( xSemaphore );
    }

    
}

static void process_message(char * input_buffer,int input_number)
{

    // parse for operator
    // dispatch command table
    
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "freertos/FreeRTOS.h"
#include <esp_types.h>
#include "freertos/task.h"
#include "console_input_server.h"

#include "console_output_server.h"

static TaskHandle_t xHandle = NULL;
static void input_processor ( void * pvParameters );
bool console_input_init(void)
{
    
   xTaskCreate( input_processor, "USB_SER_INPUT", 4000, NULL, 10, &xHandle );
    configASSERT( xHandle );
    return true;    
    
    
    
    
}

bool console_input_attach_command(char *command,COMMAND_PROCESS command_function)
{
    return true;
    
    
}

static void input_processor ( void * pvParameters )
{
 for( ;; )
 {
      vTaskDelay( 10 * portTICK_PERIOD_MS );
     
      console_output("input processor task\n");
     
 }
}

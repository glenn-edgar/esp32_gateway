
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "watchdog.h"
#include "gpio.h"
#include "debounce_ctrl.h"
#include "mqtt_ctrl.h"
#include "msg_dict_stream.h"
#include "app_input_read_config.h"
#include "chain_flow_assembler/cf_external_functions.h"
#include "app_input_functions.h"



static uint32_t input_number;
static uint32_t *gpio_input_pins;
static DEBOUNCE_CTRL *debounce_control;





bool app_input_input_data_read(void)
{
  bool return_value;
 
  return_value = app_input_read_configuration( &input_number, &gpio_input_pins,&debounce_control );
#if 0  
  printf("************** %d %d \n",return_value, input_number);
  for(int i = 0; i< input_number;i++)
  {
      printf("gpio_input_pins %d %d \n",i,gpio_input_pins[i]);
      printf("debounce.current.level %d \n",debounce_control[i].current_level);
      printf("debounce.reference_level %d \n",debounce_control[i].reference_level);
      printf("debounce.current_count %d \n",debounce_control[i].current_count);
      printf("debounce.transition_count %d \n",debounce_control[i].transition_count);

  }
  vTaskDelay(100/ portTICK_PERIOD_MS); // allow print to display
#endif  
  return return_value;  
    
    
}

static void send_mqtt_message(int pin,int value);
bool debounce_control_update( DEBOUNCE_CTRL *debounce_ctrl, unsigned new_value);

int pat_watch_dog(unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   wdt_reset_task_time(); 
   return CF_DISABLE;   
}


int sample_switches(unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    unsigned input;
    bool     transistion;
    
    for( int i=0;i<input_number;i++)
    {
        input = gpio_read_value(gpio_input_pins[i]);
        transistion = debounce_control_update( &debounce_control[i], input);
        if(transistion == true)
        {
            send_mqtt_message(gpio_input_pins[i],input);
        }
        
    }
    return CF_DISABLE;    
}

int output_values(unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
   
   for( int i=0;i<input_number;i++)
   {
        
        send_mqtt_message(gpio_input_pins[i],debounce_control[i].reference_level);
        
        
    }
    return CF_DISABLE;    
}     
      
 


int add_watch_dog(unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    
   wdt_task_subscribe(); 
   return CF_DISABLE;     
}  


int setup_gpio(unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    // do notihing for now
    return CF_DISABLE;    
}

int wait_for_mqtt_connect(unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)

{
    MQTT_STATE  state;
    int return_value;
    state = mqtt_ctl_get_state();
    if( (state == RESTORE_SUBSCRIPTIONS) || ( state ==MQTT_OPERATIONAL_STATE ))
    {
    return_value = 1;
    }
    else
    {
        return_value = 0;
    }
    return return_value;
}    

static void send_mqtt_message(int pin,int value)
{
   MSG_PACK_ELEMENT msg_pack[5];
   char *pack_buffer;
   int pack_buffer_size;
  
   msg_dict_pack_string(&msg_pack[0],"TOPIC","INPUT/GPIO_INPUTS");      
   msg_dict_pack_unsigned_integer(&msg_pack[1],"PIN",pin );
   msg_dict_pack_unsigned_integer(&msg_pack[2],"INPUT",value);
   pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
   mqtt_clt_publish("INPUT/SWITCHES", pack_buffer,pack_buffer_size );
   free(pack_buffer);
}
   
    
    
    
    


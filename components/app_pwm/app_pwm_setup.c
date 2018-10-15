#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "driver/mcpwm.h"
#include "msgpack_rx_handler.h"
#include "app_output_read_config.h"



#define MCPWM_PIN_IGNORE    (-1)




typedef struct
{
    uint32_t unit_id;
    uint32_t pin_a;
    uint32_t pin_b; // MCPWM_PIN_IGNORE if not used
    uint32_t frequency;
    float   duty_a;
    float   duty_b;
   
}PWM_CHANNEL_T;



uint32_t      pwm_channel_number;
PWM_CHANNEL_T pwm_channel[MCPWM_TIMER_MAX];




static void app_pwm_duty( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  );
                          
static bool app_pwm_read_file_configuration( void );


static bool  app_pwm_find_set_configuration(  uint32_t data_len, char *data);
static void initialize_timer_units(void);
static bool app_pwm_read_pwm_duty( uint32_t data_len, char *data, uint32_t *timer_number,
                                   float *duty_cycle_a, float *duty_cycle_b);
                                   
static void app_pwm_change_pwm_duty( uint32_t timer_number,
                             float duty_cycle_a,
                             float duty_cycle_b );
                             
static void app_pwm_frequency( esp_mqtt_client_handle_t mqtt_client,
                                 uint32_t topic_len,
                                 char *topic, 
                                 uint32_t data_len, 
                                char *data  );
                             
 
static bool app_pwm_read_frequency( uint32_t data_len,
                      char *data,                            
                      uint32_t *timer_number,
                      uint32_t *frequency); 
                      
static void app_pwm_change_pwm_frequency(uint32_t timer_number, uint32_t frequency );


                             
void initialize_app_pwm_main(void)
{
    pwm_channel_number = 0;
    memset(pwm_channel,0,sizeof(pwm_channel));
    initialize_timer_units();
    if( app_pwm_read_file_configuration( ) == true)
    {
       mqtt_ctrl_register_subscription("OUTPUTS/PWM/DUTY", app_pwm_duty );
       mqtt_ctrl_register_subscription("OUTPUTS/PWM/FREQUENCY", app_pwm_frequency );
    }
        
}
    
static void app_pwm_duty( esp_mqtt_client_handle_t mqtt_client,
                          uint32_t topic_len,
                          char *topic, 
                          uint32_t data_len, 
                          char *data  )
{

   uint32_t timer_number;
   float duty_cycle_a; 
   float duty_cycle_b; 
   
   if( app_pwm_read_pwm_duty(data_len,
                             data,                            
                             &timer_number,
                             &duty_cycle_a,
                             &duty_cycle_b ) == true)
   {
       app_pwm_change_pwm_duty(timer_number,duty_cycle_a,duty_cycle_b );
   }       
}                          


static void app_pwm_frequency(  esp_mqtt_client_handle_t mqtt_client,
                                 uint32_t topic_len,
                                 char *topic, 
                                 uint32_t data_len, 
                                char *data  )
                                
                                {

   uint32_t timer_number;
   uint32_t frequency;

   if( app_pwm_read_frequency(data_len,
                             data,                            
                             &timer_number,
                             &frequency) == true)

   {
       app_pwm_change_pwm_frequency(timer_number,frequency );
   }       
} 
                                

static bool app_pwm_read_file_configuration( void ) 
{
    bool          return_value;
    cmp_ctx_t ctx;
    char     *buffer;
    uint32_t buffer_size = 1000;
    
       
    
    if( msgpack_rx_handler_file(&ctx,"/spiffs/IO_PWM.MPK" ,  &buffer,&buffer_size ) != true)
    {
        
         return false;
    }
   
    
    return_value = app_pwm_find_set_configuration(   buffer_size, buffer);
    
    free(buffer);
    return return_value;    
    
}

 

static void initialize_timer_units( void )
{
  PWM_CHANNEL_T *temp;

  temp = pwm_channel;
  for(int i = 0; i<MCPWM_TIMER_MAX;i++)
  {
      temp->pin_a = MCPWM_PIN_IGNORE; 
      temp->pin_b = MCPWM_PIN_IGNORE;
      if( i < 3 )
      {
          temp->unit_id = 0;
      }
      else
      {
          temp->unit_id = 1;
      }
      
      
  }      
    
}    


   
static void app_pwm_change_pwm_duty( uint32_t timer_number,
                             float duty_cycle_a,
                             float duty_cycle_b )
{
    
    
    pwm_channel[timer_number].duty_a = duty_cycle_a;
    pwm_channel[timer_number].duty_b = duty_cycle_b;
    mcpwm_set_duty(pwm_channel[timer_number].unit_id,timer_number, MCPWM_OPR_A,duty_cycle_a);  
    mcpwm_set_duty(pwm_channel[timer_number].unit_id,timer_number, MCPWM_OPR_B,duty_cycle_b);
     
 }

                      
static void app_pwm_change_pwm_frequency(uint32_t timer_number, uint32_t frequency )
{
    
    
    pwm_channel[timer_number].frequency = frequency;
    mcpwm_set_frequency(pwm_channel[timer_number].unit_id,timer_number, frequency);  
      
    
}
   
    
                          
/*
**
** MESSAGE PACK STUFF
**
**
*/

static bool app_pwm_read_frequency( uint32_t data_len,
                      char *data,                            
                      uint32_t *timer_index,
                      uint32_t *frequency)                       

{

     
     cmp_ctx_t ctx;
     msgpack_rx_handler_init(&ctx, data, data_len);

      if(msgpack_rx_handler_find_unsigned(&ctx,"timer_index", timer_index) == false)
     {
         return false;
     }  
 
     if(msgpack_rx_handler_find_unsigned(&ctx,"frequency", frequency) == false)
     {
         return false;
     } 
 
     // checking inputs
     
     if(*timer_index >=pwm_channel_number){return false;}
    
    
     
   return true;                                       
}
                      
                      
static bool app_pwm_read_pwm_duty( uint32_t data_len, char *data, uint32_t *timer_number,
                                   float *duty_cycle_a, float *duty_cycle_b)
{
     cmp_ctx_t ctx;
     msgpack_rx_handler_init(&ctx, data, data_len);

     if( msgpack_rx_handler_find_unsigned(&ctx,"timer", timer_number ) == false)
     {
         return false;
     }     
     if(msgpack_rx_handler_find_float(&ctx,"duty_a", duty_cycle_a ) == false)
     {
         return false;
     }    
     if(msgpack_rx_handler_find_float(&ctx,"duty_b", duty_cycle_b ) == false)
     {
         return false;
     }     
     // checking inputs
     
     if(*timer_number >=pwm_channel_number){return false;}
    
    
     
   return true;                                       
}


static bool app_pwm_process_buffer(uint32_t timer_id,uint32_t data_len , char *data)
{
   cmp_ctx_t ctx;
   uint32_t pin_a;
   uint32_t pin_b;
   uint32_t frequency;
   float    duty_a;
   float    duty_b;
   msgpack_rx_handler_init(&ctx, data, data_len);   

   if( msgpack_rx_handler_find_float(&ctx,"duty_a", &duty_a ) == false)
   {
       return false;
       
   }   
   if( msgpack_rx_handler_find_float(&ctx,"duty_b", &duty_b )== false)
   {
       return false;
       
   }   
   if( msgpack_rx_handler_find_unsigned(&ctx,"frequency", &frequency ) == false)
   {
       return false;
       
   }   
   if( msgpack_rx_handler_find_unsigned(&ctx,"pin_a", &pin_a ) == false)
   {
       return false;
       
   }   

    if( msgpack_rx_handler_find_unsigned(&ctx,"pin_b", &pin_b ) == false)
   {
       return false;
       
   }   
   pwm_channel[timer_id].pin_a = pin_a;
   pwm_channel[timer_id].pin_b = pin_b;
   pwm_channel[timer_id].frequency = frequency;
   pwm_channel[timer_id].duty_a = duty_a;
   pwm_channel[timer_id].duty_b = duty_b;
   mcpwm_config_t pwm_config;
   pwm_config.frequency = frequency;    
   pwm_config.cmpr_a = duty_a;       
   pwm_config.cmpr_b = duty_b;      
   pwm_config.counter_mode = MCPWM_UP_COUNTER;
   pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

   if( timer_id >= 3)
   {
      mcpwm_gpio_init(1, timer_id*2, pin_a);
      mcpwm_gpio_init(1, (timer_id*2)+1, pin_b);
      mcpwm_init(1, timer_id-3, &pwm_config); 
   }
   else
   {
      mcpwm_gpio_init(0, timer_id*2, pin_a);
      mcpwm_gpio_init(0, (timer_id*2)+1, pin_b);
      mcpwm_init(0, timer_id, &pwm_config); 
   }
       
   
    
   return true;
    
}



static bool  app_pwm_find_set_configuration(  uint32_t data_len, char *data)
{ 

   
    cmp_ctx_t ctx;
    uint32_t timer_count;
    uint32_t binary_length[ MCPWM_TIMER_MAX ];
    char     *binary_buffer[ MCPWM_TIMER_MAX];

    msgpack_rx_handler_init(&ctx, data, data_len);

    if( msgpack_rx_handler_find_array_count(&ctx,"timers",&timer_count) != true )
    {
        return false;
    }
    if(timer_count >= MCPWM_TIMER_MAX){ return false; }
    pwm_channel_number = timer_count;
    
    
    if(msgpack_rx_handler_get_binary_array(&ctx,"timers",&timer_count,binary_length,binary_buffer) == false)
    {
        return false;
    }
    for(int i = 0; i< timer_count ;i++)
    {
       if( app_pwm_process_buffer(i, binary_length[i],binary_buffer[i]) == false)
       {
           return false;
       }
       
    }

   
    return true;       
    
    
}




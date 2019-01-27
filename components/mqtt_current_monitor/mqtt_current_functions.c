
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "watchdog.h"
#include "gpio.h"
#include "debounce_ctrl.h"
#include "mqtt_client.h"
#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"
#include "msg_dict_stream.h"
#include "mqtt_current_read_config.h"
#include "app_analog_task.h"
#include "mqtt_current_relay_power_control.h"
#include "chain_flow_assembler/cf_external_functions.h"
#include "mqtt_current_functions.h"

#define EQUIPMENT_CHANNEL 0
#define IRRIGATION_CHANNEL 1
static float max_equipment_current  = 0.;
static float max_irrigation_current = 0.;
static bool  subscription_state = false;


static void get_max_currents(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void clear_max_currents(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void read_currents(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void enable_equipment_relay(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void enable_irrigation_relay(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void disable_equipment_relay(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void disable_irrigation_relay (esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                
static void read_relay_states(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  );
                                

int mqtt_current_register_subscriptions(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    mqtt_ctrl_register_subscription("INPUT/MQTT_CURRENT/GET_MAX_CURRENTS", get_max_currents );
    mqtt_ctrl_register_subscription("INPUT/MQTT_CURRENT/CLEAR_MAX_CURRENTS", clear_max_currents );
    mqtt_ctrl_register_subscription("INPUT/MQTT_CURRENT/READ_CURRENT", read_currents );
    mqtt_ctrl_register_subscription("OUTPUT/MQTT_CURRENT/ENABLE_EQUIPMENT_RELAY", enable_equipment_relay );
    mqtt_ctrl_register_subscription("OUTPUT/MQTT_CURRENT/ENABLE_IRRIGATION_RELAY", enable_irrigation_relay );
    mqtt_ctrl_register_subscription("OUTPUT/MQTT_CURRENT/DISABLE_EQUIPMENT_RELAY", disable_equipment_relay );
    mqtt_ctrl_register_subscription("OUTPUT/MQTT_CURRENT/DISABLE_IRRIGATION_RELAY", disable_irrigation_relay );
    mqtt_ctrl_register_subscription("OUTPUT/MQTT_CURRENT/READ_RELAY_STATES", read_relay_states );
   
    return CF_DISABLE;
    
}

int mqtt_current_set_subscription_flag(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
      
      
   subscription_state = true;   
   return CF_DISABLE;      
      
}





int mqtt_current_initialize_ref_max_currents(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    max_equipment_current  = 0.;
    max_irrigation_current = 0.;    
    return CF_DISABLE;
    
    
    
}













int mqtt_current_monitor_equipment_relay(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
{
    MSG_PACK_ELEMENT msg_pack[5];
    char *pack_buffer;
    int pack_buffer_size;

    float *average_currents;

    average_currents = app_analog_get_analog_channels(); 
    if(average_currents[EQUIPMENT_CHANNEL] > max_equipment_current)
    {
        max_equipment_current = average_currents[EQUIPMENT_CHANNEL];
    }
    if( average_currents[EQUIPMENT_CHANNEL] > mqtt_current_get_equipment_level())
    {
        mqtt_relay_set_equipment_off();
        msg_dict_pack_string(&msg_pack[0],"TOPIC","OUTPUT/MQTT_CURRENT/EQUIPMENT_RELAY_TRIP");      
        msg_dict_pack_float(&msg_pack[1],"CURRENT_VALUE",average_currents[EQUIPMENT_CHANNEL] );
        msg_dict_pack_float(&msg_pack[2],"LIMIT_VALUE",mqtt_current_get_equipment_level());
        pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
        mqtt_clt_publish("OUTPUT/MQTT_CURRENT/EQUIPMENT_RELAY_TRIP", pack_buffer,pack_buffer_size );
        free(pack_buffer);        
        
        
    }        
    return CF_DISABLE;
    
    
    
}









int mqtt_current_monitor_irrigation_relay(CHAIN_FLOW_HANDLE *cf, unsigned link_id, unsigned param_1,
  unsigned param_2, unsigned param_3, unsigned event, unsigned data)
  
{
    MSG_PACK_ELEMENT msg_pack[5];
    char *pack_buffer;
    int pack_buffer_size;

    float *average_currents;

    average_currents = app_analog_get_analog_channels(); 
    if(average_currents[IRRIGATION_CHANNEL] > max_irrigation_current)
    {
        max_irrigation_current = average_currents[IRRIGATION_CHANNEL];
    }
    if( average_currents[IRRIGATION_CHANNEL] > mqtt_current_get_irrigation_level())
    {
        mqtt_relay_set_irrigation_off();
        msg_dict_pack_string(&msg_pack[0],"TOPIC","OUTPUT/MQTT_CURRENT/IRRIGATION_RELAY_TRIP");      
        msg_dict_pack_float(&msg_pack[1],"CURRENT_VALUE",average_currents[IRRIGATION_CHANNEL] );
        msg_dict_pack_float(&msg_pack[2],"LIMIT_VALUE",mqtt_current_get_irrigation_level());
        pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
        mqtt_clt_publish("OUTPUT/MQTT_CURRENT/IRRIGATION_RELAY_TRIP", pack_buffer,pack_buffer_size );
        free(pack_buffer);        
        
        
    }        
    return CF_DISABLE;
    
    
    
} 
  
  
  
  
static void get_max_currents(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
{
    

   MSG_PACK_ELEMENT msg_pack[5];
   char *pack_buffer;
   int pack_buffer_size;

   

   msg_dict_pack_string(&msg_pack[0],"TOPIC","INPUT/MQTT_CURRENT/MAX_CURRENTS");      
   msg_dict_pack_float(&msg_pack[1],"MAX_EQUIPMENT_CURRENT", max_equipment_current );
   msg_dict_pack_float(&msg_pack[2],"MAX_IRRIGATION_CURRENT", max_irrigation_current);
   pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
   mqtt_clt_publish("INPUT/MQTT_CURRENT/MAX_CURRENTS", pack_buffer,pack_buffer_size );
   free(pack_buffer);
    
}
                     
                                
static void clear_max_currents(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                               
{
    
    max_equipment_current  = 0.;
    max_irrigation_current = 0.;
    
}                                
  
static void read_currents(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                                                              
{
   MSG_PACK_ELEMENT msg_pack[5];
   char *pack_buffer;
   int pack_buffer_size;
   float *average_currents;

   average_currents = app_analog_get_analog_channels();   
   msg_dict_pack_string(&msg_pack[0],"TOPIC","INPUT/MQTT_CURRENT/CURRENTS");      
   msg_dict_pack_float(&msg_pack[1],"MAX_EQUIPMENT_CURRENT", average_currents[EQUIPMENT_CHANNEL] );
   msg_dict_pack_float(&msg_pack[2],"MAX_IRRIGATION_CURRENT", average_currents[IRRIGATION_CHANNEL]);
   pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
   mqtt_clt_publish("INPUT/MQTT_CURRENT/CURRENTS", pack_buffer,pack_buffer_size );
   free(pack_buffer);
    
}
                                
                                
static void enable_equipment_relay(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                               
{
    
     mqtt_relay_set_equipment_on();
    
}
                                
static void enable_irrigation_relay(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                                                
{
    
    mqtt_relay_set_irrigation_on(  );
    
}
                                
static void disable_equipment_relay(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                            
{
    
   
   mqtt_relay_set_equipment_off( );



    
}                                
                                
static void disable_irrigation_relay (esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                                               
{
    
    mqtt_relay_set_irrigation_off();
    
}
                                
static void read_relay_states(esp_mqtt_client_handle_t mqtt_client,
                                uint32_t topic_len,
                                char *topic, 
                                uint32_t data_len, 
                                char *data  )
                                              
{
    

   MSG_PACK_ELEMENT msg_pack[5];
   char *pack_buffer;
   int pack_buffer_size;
   bool equipment_state_value;
   bool irrigation_state_value;
   
   mqtt_current_get_relay_states( &equipment_state_value, &irrigation_state_value);
   msg_dict_pack_string(&msg_pack[0],"TOPIC","OUTPUT/MQTT_CURRENT/RELAY_STATE");      
   msg_dict_pack_boolean(&msg_pack[1],"EQUIPMENT_STATE", equipment_state_value );
   msg_dict_pack_boolean(&msg_pack[2],"IRRIGATION_STATE",irrigation_state_value);
   pack_buffer = msg_dict_stream( &pack_buffer_size,3,msg_pack);
   mqtt_clt_publish("OUTPUT/MQTT_CURRENT/RELAY_STATE", pack_buffer,pack_buffer_size );
   free(pack_buffer);
}
    

  


                            

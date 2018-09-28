#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"


#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_ctrl_load_setup_data.h"
#include "wifi_station_setup.h"

#include "mqtt_ctrl.h"
#include "mqtt_ctrl_subscription.h"


#define SUBSCRIPTION_MAX 32
                        
                                
typedef struct 
{
    SUBSCRITION_FUNCTION sub_function;
    char *topic;  // variable will be malloced
}SUBSCRIPTION_TABLE;

static uint32_t subscription_number;
static SUBSCRIPTION_TABLE subscription_table[SUBSCRIPTION_MAX];      
                          
static char *base_topic;                     
   
static bool check_for_duplicates_sub_table(char *topic );

   
void mqtt_initialize_subscriptions(char *base_top)
{
      base_topic = base_top;
      subscription_number = 0;    
      memset(subscription_table,0,sizeof(subscription_table));
      // add built in commands
    
}

void mqtt_ctrl_register_subscription(char *topic, SUBSCRITION_FUNCTION topic_function )
{
      uint32_t size;
      char *ref_topic;
      
      if( subscription_number >= SUBSCRIPTION_MAX )
      {
          abort(); // ran out of table space
      }
      if( check_for_duplicates_sub_table(topic) == true)
      {
          abort(); // all ready in table
      }
      size = strlen(base_topic)+strlen(topic)+1;
      ref_topic = malloc(size);
      strcpy(ref_topic,base_topic);
      strcat(ref_topic,topic);
      subscription_table[subscription_number].sub_function = topic_function;
      subscription_table[subscription_number].topic = ref_topic;
      mqtt_clt_subscribe(ref_topic);
      
    
    
}



void mqtt_ctl_restore_subscriptions(esp_mqtt_client_handle_t mqtt_client)
{
    for(int i = 0;i<subscription_number;i++ )
    {
       mqtt_clt_subscribe(subscription_table[i].topic);
        
    }
    
}

void  mqtt_ctl_handle_data(esp_mqtt_client_handle_t mqtt_client,
                           uint32_t topic_len,
                           char *topic, 
                           uint32_t data_len, 
                           char *data  )
{
    
    
    for(int i=0;i<subscription_number;i++)
    {
        if(strlen(subscription_table[i].topic)!= topic_len)
        {
            break;
        }
        if( strncmp(subscription_table[i].topic,topic,topic_len) == 0)
        {
            subscription_table[i].sub_function(mqtt_client, topic_len, topic,data_len,data);
            return;
            
        }
                   
        
        
    }
    abort(); // topic function not detected
    
}



/*

Static Data

*/



static bool check_for_duplicates_sub_table(char *topic )
{
  
   int  topic_len;
   
   topic_len = strlen(topic);
   for(int i=0;i<subscription_number;i++)
    {
        if(strlen(subscription_table[i].topic)!= topic_len)
        {
            
            break;
        }
        if( strncmp(subscription_table[i].topic,topic,topic_len) == 0)
        {
           return true;
            
        }
                   
        
        
    }
   return false;
}   
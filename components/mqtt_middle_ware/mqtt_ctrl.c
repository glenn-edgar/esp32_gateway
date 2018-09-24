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

static TaskHandle_t xHandle = NULL;
static SemaphoreHandle_t xSemaphore = NULL;
static esp_mqtt_client_handle_t mqtt_client;



static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
static esp_mqtt_client_config_t mqtt_cfg;
 


static void mqtt_client_task( void * pvParameters );





void initialize_mqtt_client_handler( void)
{
  vSemaphoreCreateBinary( xSemaphore );  
  xTaskCreate( mqtt_client_task, "MQTT_CLIENT_TASK",16000, NULL, 20, &xHandle );
    configASSERT( xHandle );

    
}

//bool mqtt_ctl_register_subscription( TBD);
//bool mqtt_ctl_publish_topic( char *topic, char *data );



/*
********************************  static functions *****************************************
*/





static MQTT_STATE mqtt_ctl_state;
#define BASE_TOPIC_SIZE 256
static char   base_topic[BASE_TOPIC_SIZE];
static uint32_t base_topic_size;





static void mqtt_ctl_wait_for_wifi_connect(void); 
static void mqtt_ctl_setup_connection(void);
static void mqtt_ctl_restore_subscriptions(void);
static void  mqtt_ctl_handle_data(uint32_t topic_len,char *topic, uint32_t data_len, char *data  );  



void mqtt_ctl_change_state( MQTT_STATE new_state)
{
   xSemaphoreTake( xSemaphore, ( TickType_t ) 10 );
   mqtt_ctl_state = new_state;
   xSemaphoreGive( xSemaphore );

   
}

MQTT_STATE mqtt_ctl_get_state(void)
{
   MQTT_STATE return_value;
   
   xSemaphoreTake( xSemaphore, ( TickType_t ) 10 );
   return_value = mqtt_ctl_state;
   xSemaphoreGive( xSemaphore );
   return return_value;

} 



// Task to be created.
static void mqtt_client_task( void * pvParameters )
{
   mqtt_ctl_change_state( LOAD_CONFIGURATION);
   while(1)
   {       

      MQTT_STATE state;
      state = mqtt_ctl_get_state();
   
      switch(state)
      {
          case LOAD_CONFIGURATION:
              memset(&mqtt_cfg,0,sizeof(mqtt_cfg));
              base_topic_size =sizeof(base_topic);
              mqtt_ctl_load_configuration(&mqtt_cfg, base_topic, &base_topic_size);
              break;
           
          case WAIT_FOR_WIFI_CONNECT:
             mqtt_ctl_wait_for_wifi_connect();
             break;
           
          case SETUP_MQTT_CONNECTION:
              mqtt_ctl_setup_connection();
             break;
           
          case WAIT_FOR_MQTT_CONNECT: // event call back will change state
              break;
           
          case RESTORE_SUBSCRIPTIONS:
              mqtt_ctl_restore_subscriptions();
              mqtt_ctl_state = MQTT_OPERATIONAL_STATE;
              break;
           
          case MQTT_OPERATIONAL_STATE: // event_callback will change this state
              break;
          case MQTT_ERROR_STATE:
           vTaskDelay(10000/ portTICK_PERIOD_MS); // error state prevent excessive cpu cycles
           break;
       
       }
       vTaskDelay(10/ portTICK_PERIOD_MS);
   }
}   
       
       
       
   


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    //esp_mqtt_client_handle_t client = event->client;
    //int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            printf( "MQTT_EVENT_CONNECTED \n");
            mqtt_ctl_change_state(RESTORE_SUBSCRIPTIONS);
            break;
        case MQTT_EVENT_DISCONNECTED:
            printf( "MQTT_EVENT_DISCONNECTED \n");
            mqtt_ctl_change_state(WAIT_FOR_WIFI_CONNECT);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        case MQTT_EVENT_DATA:
            printf( "MQTT_EVENT_DATA \n");
            // need topic handler
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            mqtt_ctl_handle_data(event->topic_len, event->topic,event->data_len, event->data  );
            break;
        case MQTT_EVENT_ERROR:
            printf( "MQTT_EVENT_ERROR \n");
            break;
    }
    return ESP_OK;
}


static void mqtt_ctl_wait_for_wifi_connect(void)
{
    if( wifi_get_wifi_setup_status() == true)
    {
        mqtt_ctl_change_state( SETUP_MQTT_CONNECTION);
        
    }
    
}
static void mqtt_ctl_setup_connection(void)
{
     printf("uri %s \n",  mqtt_cfg.uri );
     printf("host %s \n", mqtt_cfg.host);
     printf("port  %d \n", mqtt_cfg.port);
     printf("user name %s \n", mqtt_cfg.username);
     printf("password %s \n", mqtt_cfg.password );
     mqtt_ctl_change_state(WAIT_FOR_MQTT_CONNECT);
    mqtt_cfg.event_handle = mqtt_event_handler;
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
    
}
static void mqtt_ctl_restore_subscriptions(void)
{
    ;
    
}


static void  mqtt_ctl_handle_data(uint32_t topic_len,char *topic, uint32_t data_len, char *data  )
{
    ;
    
}



#if 0
static const char *TAG = "MQTT_SAMPLE";

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}



static void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://iot.eclipse.org",
        .event_handle = mqtt_event_handler,
        // .user_context = (void *)your_context
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void app_main()
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    nvs_flash_init();
    wifi_init();
    mqtt_app_start();
}


            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            
                        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
#endif
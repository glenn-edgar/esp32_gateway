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
#include "mqtt_ctrl.h"
#include "msgpack_rx_handler.h"
#include "mqtt_ctrl_load_setup_data.h"

#define BUFFER_SIZE    1024
#define URI_SIZE         32
#define USERNAME_SIZE   32
#define PASSWORD_SIZE    32
#define CERT_PEM_SIZE  5522 




static uint32_t       cert_pem_size;
static char       cert_pem[CERT_PEM_SIZE];
static char       uri[URI_SIZE];
static char       username[USERNAME_SIZE];
static char        password[PASSWORD_SIZE];
static uint32_t   port;


void mqtt_ctl_load_configuration(esp_mqtt_client_config_t *mqtt_cfg,char *base_topic, uint32_t *topic_size )
{

    bool ret;
    char     *buffer;
    uint32_t  buffer_size;
   
    cmp_ctx_t ctx;
    
    buffer_size = BUFFER_SIZE; 
    
    ret = msgpack_rx_handler_file(&ctx,"/spiffs/MQTT.MPK",&buffer,&buffer_size );
    if(ret == false)
    {
        return ;
    }
   
    buffer_size = sizeof(uri);
    ret = msgpack_rx_handler_find_string( &ctx,"URI", uri, &buffer_size );
    if(ret == false)
    {
        goto end;
        
    }
    
    ret = msgpack_rx_handler_find_unsigned(&ctx,"PORT", &port );
    if(ret == false)
    {
        goto end;
        
    }
    
    cert_pem_size = CERT_PEM_SIZE;
    ret = msgpack_rx_handler_find_binary(&ctx,"CERTIFICATE", cert_pem, &cert_pem_size);
    if(ret == false)
    {
        goto end;
        
    }
    
    buffer_size = sizeof(username);
    ret = msgpack_rx_handler_find_string( &ctx,"USER_NAME", username, &buffer_size );
    
    if( ret == false )
    {
       goto end;
    }
    
    buffer_size = sizeof(password);
    ret = msgpack_rx_handler_find_string(&ctx, "PASSWORD", password, &buffer_size );
    if( ret == false)
    {
       goto end;
    }
    
    ret = msgpack_rx_handler_find_string(&ctx, "TOPIC", base_topic, topic_size );
    if( ret == false)
    {
       goto end;
    } 
    mqtt_cfg->port = port;
    mqtt_cfg->uri = uri;
    mqtt_cfg->username = username;
    mqtt_cfg->password = password;
    mqtt_cfg-> cert_pem  = cert_pem;  
     
end:
    free(buffer);   
}

 
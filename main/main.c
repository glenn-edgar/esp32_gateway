#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"

#include <esp_types.h>



#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_vfs_fat.h"
#include "esp_task_wdt.h"
#include "esp_eth.h"
#include "esp_log.h"

#include "driver/uart.h"
#include "rom/uart.h"
#include "flash/flash_setup.h"
#include "sd_card/sd_card.h"

#include "configuration.h"

#if _ENABLE_ETHERNET_ 
#include "ethernet/olimex_ethernet.h"
#endif

#if _ENABLE_WATCHDOG_
#include "watchdog/watchdog.h"

#endif

#if _ENABLE_HEART_BEAT_

#include "gpio/gpio.h"

#endif

#if _ENABLE_WIFI_
#include "wifi/wifi_station_setup.h"
#endif
#include "flash/configuration_data.h"

#define TAG "OLIMEX"






                                
void app_main(void)
{
   flash_initialize_flash();
#if _ENABLE_HEART_BEAT_ 
    int heartbeat_toggle;
    
#endif
    
    printf("-------REBOOT------------\n");
#if _ENABLE_ETHERNET_ 
    initEthernet(config_get_lan_host_name());
#endif

#if _ENABLE_WIFI_
    wifi_init_sta(config_get_wifi_host_name(),config_get_ssid(),config_get_password());
#endif

#if _ENABLE_SD_CARD_
   initialize_sd_card();
   test_file_system();
   
   

#endif

#if _ENABLE_WATCHDOG_ 
    printf("current task handle %p\n",xTaskGetCurrentTaskHandle());
    initialize_watchdog(3, true);
    wdt_task_subscribe();

#endif
#if _ENABLE_HEART_BEAT_
     heartbeat_toggle = 1;
     gpio_config_output_pin( HEART_BEAT );
#endif

    while(1)
    {

#if _ENABLE_HEART_BEAT_      
       heartbeat_toggle = (heartbeat_toggle^0xffff)&0x0001;
       gpio_set_value(HEART_BEAT,heartbeat_toggle);
#endif    
      wdt_reset_task_time();   
      vTaskDelay(1000 / portTICK_PERIOD_MS);
        
       
        
    }
}
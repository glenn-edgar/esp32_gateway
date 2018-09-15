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
#include <freertos/task.h>

#include "driver/uart.h"
#include "rom/uart.h"
#include "flash_setup.h"
#include "sd_card.h"
#include "message_processor.h"


#include "configuration.h"

#if _ENABLE_ETHERNET_ 
#include "ethernet/olimex_ethernet.h"
#endif

#if _ENABLE_WATCHDOG_
#include "watchdog.h"

#endif

#if _ENABLE_HEART_BEAT_

#include "gpio.h"

#endif

#if _ENABLE_WIFI_
#include "wifi_station_setup.h"
#endif


#include "internal_temp.h"

#include "spiffs_setup.h"
#include "console_output_server.h"
#include "console_input_server.h"
#include "ble_custom_beacon.h"


                                
void app_main(void)
{
   MSG_PACK_ELEMENT msg_pack[5];

#if _ENABLE_HEART_BEAT_ 
    int heartbeat_toggle;
    
#endif
    
    printf("-------REBOOT------------\n");
    
    if( initialize_spiffs() == false )
    {
      // bad flash cannot go further
      esp_restart();    
        
    }
#if _ENABLE_SD_CARD_
    initialize_sd_card();
    //test_file_system();
   
   

#endif
    flash_initialize_flash();
    init_message_processor();
    console_output_server();
   initialize_console_input_server();
#if _ENABLE_ETHERNET_ 
    initEthernet();
#endif

#if _ENABLE_WIFI_
    wifi_init_sta();
#endif

  ibeacon_custom_setup();
  ibeacon_custom_start(1000,0x5555,0xaaaa);

#if _ENABLE_WATCHDOG_ 
    printf("current task handle %p\n",xTaskGetCurrentTaskHandle());
    initialize_watchdog(5, true);
    wdt_task_subscribe();

#endif
#if _ENABLE_HEART_BEAT_
     heartbeat_toggle = 1;
     gpio_config_output_pin( HEART_BEAT );
#endif

      
      msg_dict_pack_string(&msg_pack[0],"TOPIC","HEART_BEAT");      
      msg_dict_pack_float(&msg_pack[1],"CHIP_TEMP", ( temprature_sens_read() -32)*9/5);
      msg_dict_pack_unsigned_integer(&msg_pack[2],"FREE_HEAP",esp_get_free_heap_size());
      console_output_structured_data(3, msg_pack);
      printf("made it here \n");
    while(1)
    {
      
      
#if 0
      printf("high water stack mark %d \n",uxTaskGetStackHighWaterMark( NULL ));
      printf("heap space %d \n",esp_get_free_heap_size());
#endif
#if _ENABLE_HEART_BEAT_      
       heartbeat_toggle = (heartbeat_toggle^0xffff)&0x0001;
       gpio_set_value(HEART_BEAT,heartbeat_toggle);
#endif 
#if 0   
      msg_dict_pack_string(&msg_pack[0],"TOPIC","HEART_BEAT");      
      msg_dict_pack_float(&msg_pack[1],"CHIP_TEMP", ( temprature_sens_read() -32)*9/5);
      msg_dict_pack_unsigned_integer(&msg_pack[2],"FREE_HEAP",esp_get_free_heap_size());
      console_output_structured_data(3, msg_pack);
#endif
      wdt_reset_task_time();
      vTaskDelay(4000 / portTICK_PERIOD_MS);
        
       
        
    }
}


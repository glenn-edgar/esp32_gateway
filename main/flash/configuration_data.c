
#include "configuration_data.h"
#include "../configuration.h"

#if _ENABLE_ETHERNET_
char *config_get_lan_host_name(void)
{
    
   return "lan_host_name" ;   
    
    
}

#endif


#if _ENABLE_WIFI_

char *config_get_ssid(void)
{
    
  //return "RaspberryPI_Router";
  return "onyx_1_G" ; 
    
}

char *config_get_password(void)
{
    
    return "ready2go";
    
}

char *config_get_wifi_host_name(void)
{
    return "wifi_host_name";
    
}
#endif

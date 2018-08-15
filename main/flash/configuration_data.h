

#ifndef _CONFIGURATION_DATA_H_
#define _CONFIGURATION_DATA_H_

#if _ENABLE_ETHERNET_
char *config_get_lan_host_name(void);
#endif

#if _ENABLE_WIFI_
char *config_get_ssid(void);


char *config_get_password(void);


char *config_get_wifi_host_name(void);
#endif


#endif
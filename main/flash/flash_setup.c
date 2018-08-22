
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "sd_card.h"
#include "../spiffs_setup.h"
#include <dirent.h>

static void transfer_files(void);
static void transfer_file(char *filename);
static bool match_extension(char *filename);
void flash_initialize_flash(void )
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    
    if(is_sd_card_mounted() == true )
    {
      transfer_files() ; 
        
        
    }        
    // load specific files
    
    
    
}

static void transfer_files(void)
{
   struct dirent *de;  // Pointer for directory entry
   DIR *dr = opendir(sd_get_prefix());
   if (dr == NULL)  // opendir returns NULL if couldn't open directory
   {
        printf("Could not open current directory \n" );
        return;
   }
   printf("direcotry opened 7777777777777777777 \n");
  while ((de = readdir(dr)) != NULL)
  {
            printf("name %s\n", de->d_name);
            if( match_extension(de->d_name) == true )
            {
                
                transfer_file(de->d_name);
            }
           
  }
   closedir(dr);   
    
}
 
static bool match_extension(char *filename)
{
    int length;
    char *pointer;
    length = strlen(filename);
    pointer = filename+length-3;
    if( strcmp(pointer,"MPK") == 0 )
    {
        return true;
    }
    return false;
    
    
} 
static void transfer_file(char *filename)
{
    char *sd_file_name;
    char *spiffs_name;
    char *buffer;
    FILE *from_fp;
    FILE *to_fp;
    int nread;
    int nwrite;
    
    buffer = (char *)malloc(1024);
    sd_file_name = (char *)malloc(32);
    spiffs_name = (char *)malloc(32);
    
    strcpy(sd_file_name, sd_get_prefix());
    strcat( sd_file_name, filename);
    strcpy(spiffs_name, spiffs_get_prefix());
    strcat( spiffs_name, filename);
    
    from_fp = fopen(sd_file_name,"rb");
    to_fp = fopen(spiffs_name,"wb");
    
    printf("sd_file_name %s \n",sd_file_name);
    printf("spiffs_name %s \n",spiffs_name);
    
    
    nread = fread(buffer,1,1024,from_fp);
    nwrite = 0;
    if( nread > 0)
    {
        nwrite = fwrite(buffer,1,nread,to_fp);
        
    }
    printf("nread %d \n",nread);
    printf("nwrite %d \n",nwrite);
    fclose(from_fp);
    fclose(to_fp);
   
    
    free(buffer);
    free(sd_file_name);
    free(spiffs_name);
    
    
}    
    
    
    
#if 0

fd_from = fopen(from, "rb")
fread(buf, 1, sizeof(buf), fd_from), nread > 0)
 nwritten = fwrite(out_ptr, 1, nread, fd_to);
 fclose(fd_to) 
#endif  
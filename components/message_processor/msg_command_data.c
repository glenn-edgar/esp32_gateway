#include <esp_types.h>
#include <stdio.h>
#include "cmp.h"
#include "msgpack_utillities.h"
#include "msg_dict_stream.h"
#include "msg_command_cmd_table.h"
#include "msg_command_data.h"
#include "esp_system.h"
#include <dirent.h>
#include "msgpack_block_reader.h"

#define FILE_MOUNT_SIZE 16
#define FILE_SIZE 33
#define DATA_SIZE 2000

#define check_return(return_value )\
    if(return_value != true){ return false; }

    
static void add_commands(void);
/*
    build in commands

*/
static bool file_write( int *msg_pack_number, 
                        MSG_PACK_ELEMENT **msg_pack, 
                         
                        char* msg_data, 
                        int msg_data_size  );
static bool file_read( int *msg_pack_number, 
                       MSG_PACK_ELEMENT **msg_pack,                                               
                       char* msg_data, 
                       int msg_data_size  );
static bool file_delete( int *msg_pack_number, 
                         MSG_PACK_ELEMENT **msg_pack,                                                 
                         char* msg_data, 
                        int msg_data_size  );
static bool file_rename( int *msg_pack_number, 
                         MSG_PACK_ELEMENT **msg_pack,                                                 
                         char* msg_data, 
                        int msg_data_size  );
static bool file_directory( int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack,                                                       
                          char* msg_data, 
                         int msg_data_size  );
static bool reboot( int *msg_pack_number, 
                       MSG_PACK_ELEMENT **msg_pack,                                                
                       char* msg_data, 
                       int msg_data_size  );
static bool heap_space( int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack,                                                      
                          char* msg_data, 
                          int msg_data_size  );
static bool list_commands(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                                                     
                          char* msg_data, 
                          int msg_data_size  );
    
 static bool find_command_data(struct cmp_ctx_s *ctx,
                       char **command_data, uint32_t *command_size,
                       char **msgpack_data, uint32_t *msgpack_size);  

static bool get_wifi_mac_address(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  );                       
    

void msg_command_data_initialize(void)
{
    initialize_command_table( );
    add_commands();
    
    
    
}

bool msg_command_process_packet(int *msg_pack_number, 
                                 MSG_PACK_ELEMENT **msg_pack,
                                char * input_packet,int size )
{
    cmp_ctx_t *ctx;
    bool return_value;
    char *command_data;
    uint32_t command_size;
    char *msgpack_data;
    uint32_t msgpack_size;
    PROCESS_COMMAND_T command_function;
    
    *msg_pack_number = 0;
   
    ctx = msgpack_dict_scan_init( input_packet, size );
    
    return_value = find_command_data(ctx,&command_data,&command_size,
                                      &msgpack_data,&msgpack_size );
    
    if( return_value == true)
    {
        
       
       
        command_function =  msg_command_find_command(command_data,command_size);
        if( command_function != NULL )
        {
            
            return_value = command_function( msg_pack_number, msg_pack,msgpack_data,msgpack_size);
        }
 
        
    }
    else
    {
        printf("command not found \n");
    }
    
    return return_value;
}    
    
    
static bool find_command_data(struct cmp_ctx_s *ctx,
                       char **command_data, uint32_t *command_size,
                       char **msgpack_data, uint32_t *msgpack_size)
{
   bool return_value;
   uint32_t map_size;
   int   leave_count = 3;
  
   char *temp_pointer;
   uint32_t   temp_size;
   
   return_value =  cmp_read_map(ctx, &map_size);
   if( return_value != true){ return return_value;}
   
  
   for( int i= 0; i<map_size; i++ )
   {
       //printf("loop index %d\n",i);
       return_value =  msgpack_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       
       check_return(return_value);
       if( ctx_strcmp(COMMAND_FIELD,temp_pointer,temp_size ) == true)
       {
           //printf("found command \n");
           return_value = msgpack_get_bin_data_ptr(ctx,(void **)command_data,command_size );
           check_return(return_value);
           leave_count = leave_count &0x2;
           
       }
       else if( ctx_strcmp(DATA_FIELD,temp_pointer,temp_size ) == true)
       {
           
           //printf("found data \n");
           return_value = msgpack_scoop_field(ctx,(void **)msgpack_data,msgpack_size);
           
           check_return(return_value);
           leave_count = leave_count &1;
           
       }
       else
       {
          printf("should not happen \n");
          msgpack_skip_field(ctx); 
           
       }
       if(leave_count == 0)
       {
           return true;
       }    
 
   }
   
   return false;    
}      
 
/*
    storing built in command
*/

static void add_commands(void)
{
   msg_command_add_command("FILE_WRITE",file_write) ;
   msg_command_add_command("FILE_READ",file_read)   ; 
   msg_command_add_command("FILE_DELETE",file_delete) ; 
   msg_command_add_command("FILE_DIR",file_directory) ; 
   msg_command_add_command("FILE_RENAME",file_rename);
   msg_command_add_command("REBOOT",reboot) ; 
   msg_command_add_command("HEAP_SPACE",heap_space);  
   msg_command_add_command("LIST_COMMANDS",list_commands);
   msg_command_add_command("GET_WIFI_MAC_ADDRESS",get_wifi_mac_address);   
                           
    
}

static  uint8_t mac_addr[7];
static bool get_wifi_mac_address(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
  
   MSG_PACK_ELEMENT *temp;
   memset(mac_addr,0,sizeof(mac_addr));
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*3);
   *msg_pack = temp;
   *msg_pack_number = 3;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "WIFI_MAC_ADDRESS");
   esp_efuse_mac_get_default(mac_addr);
  
   msg_dict_pack_binary(&temp[2],"DATA", mac_addr, 6); 
   
   
   return true;
}


static bool file_write(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack,  
                          char* msg_data, 
                          int msg_data_size  )
{
    
    CMP_READ_BLOCK_T cmpr;
    uint32_t size =  33;
    char *temp_pointer;
    uint32_t temp_length;
    FILE *spiffs_file;
    char *file_name;
    MSG_PACK_ELEMENT *temp;
    int nwrite;

    
    *msg_pack_number = 0;
 

    
    
    
    
    file_name = malloc(size);
    memset(file_name,0,sizeof(size));

    msgpack_block_init(&cmpr,msg_data, msg_data_size);
    
    size -= strlen(file_name);
    printf("file write \n");
    if( msgpack_block_find_string(&cmpr,"FILE_NAME", file_name, &size) != true )
    {
       printf("file name not found \n");
       return false;
        
    }
    
    //printf("file name found %s %d \n\n",file_name,size);
    if( msgpack_block_find_object(&cmpr,"FILE_DATA", &temp_pointer, &temp_length) != true)
    {
       printf("file data not found \n");
       return false;
        
    }
    //printf("file_data found %d \n",temp_length);
    spiffs_file = fopen(file_name,"wb");
    
    if(spiffs_file != NULL)
    {
      nwrite = fwrite(temp_pointer,1,temp_length,spiffs_file);
      //printf("nwrite %d \n",nwrite);
      temp = malloc(sizeof(MSG_PACK_ELEMENT)*5);
      *msg_pack = temp;
      *msg_pack_number = 5;
      msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
      msg_dict_pack_string(&temp[1],"COMMAND", "FILE_WRITE");
      msg_dict_pack_map(&temp[2],"DATA",2);
      msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
      msg_dict_pack_unsigned_integer(&temp[4],"BYTES_WROTE", nwrite);
      fclose(spiffs_file);
     return true;
    }
    return false;
}    
   

static bool file_read(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    
    CMP_READ_BLOCK_T cmpr;
    uint32_t size;
    char *data_ptr;
    uint32_t data_length;
    FILE *spiffs_file;
    char *file_name;
    MSG_PACK_ELEMENT *temp;
    int nread;

    
    *msg_pack_number = 0;
 

    
    
    
    
    file_name = malloc(FILE_SIZE);
    memset(file_name,0,sizeof(size));
    size = FILE_SIZE;
    data_ptr = malloc(DATA_SIZE);
    memset(data_ptr,0,DATA_SIZE);
    data_length = DATA_SIZE;
    
    msgpack_block_init(&cmpr,msg_data, msg_data_size);
    
    
    printf("file read \n");
    if( msgpack_block_find_string(&cmpr,"FILE_NAME", file_name, &size) != true )
    {
       printf("file name not found \n");
       return false;
        
    }
    

    spiffs_file = fopen(file_name,"rb");
    
    if(spiffs_file != NULL)
    {
      nread = fread(data_ptr,1,data_length,spiffs_file);
      printf("nread %d \n",nread);
      temp = malloc(sizeof(MSG_PACK_ELEMENT)*5);
      *msg_pack = temp;
      *msg_pack_number = 5;
      msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
      msg_dict_pack_string(&temp[1],"COMMAND", "FILE_READ");
      msg_dict_pack_map(&temp[2],"DATA",2);
      msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
      msg_dict_pack_binary_dynamic(&temp[4],"FILE_DATA", data_ptr,nread);

      fclose(spiffs_file);
     return true;
    }
    return false;
}    


static bool file_delete(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    
    CMP_READ_BLOCK_T cmpr;
    uint32_t size;
    
    char *file_name;
    MSG_PACK_ELEMENT *temp;
    
    bool status;

    
    *msg_pack_number = 0;
 

    
    
    
    
    file_name = malloc(FILE_SIZE);
    memset(file_name,0,sizeof(size));
    size = FILE_SIZE;
    
    
    msgpack_block_init(&cmpr,msg_data, msg_data_size);
    
    
    printf("file read \n");
    if( msgpack_block_find_string(&cmpr,"FILE_NAME", file_name, &size) != true )
    {
       printf("file name not found \n");
       return false;
        
    }
    

    
    status = false;
    
    
    if( remove(file_name) == 0)
    {
        status = true;
    }        
    
    temp = malloc(sizeof(MSG_PACK_ELEMENT)*5);
    *msg_pack = temp;
    *msg_pack_number = 5;
    msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
    msg_dict_pack_string(&temp[1],"COMMAND", "FILE_DELETE");
    msg_dict_pack_map(&temp[2],"DATA",2);
    msg_dict_pack_string_dynamic(&temp[3],"FILE_NAME", file_name);
    msg_dict_pack_unsigned_integer(&temp[4],"STATUS", status);

    return true;
 
}    


static bool file_rename( int *msg_pack_number, 
                         MSG_PACK_ELEMENT **msg_pack,                                                 
                         char* msg_data, 
                        int msg_data_size  )
                        {
    
    CMP_READ_BLOCK_T cmpr;
    uint32_t to_size;
    uint32_t from_size;
    
    char *from_name;
    char *to_name;
    MSG_PACK_ELEMENT *temp;
    
    bool status;

    
    *msg_pack_number = 0;
 

    
    
    
    from_size = FILE_SIZE;
    from_name = malloc(FILE_SIZE);
    memset(from_name,0,from_size);
    to_size = FILE_SIZE;   
    to_name = malloc(FILE_SIZE);
    memset(to_name,0,to_size);
    
    
    msgpack_block_init(&cmpr,msg_data, msg_data_size);
    
    
    printf("file read \n");
    if( msgpack_block_find_string(&cmpr,"TO_NAME", to_name, &to_size) != true )
    {
       printf("to name not found \n");
       return false;
        
    }
    if( msgpack_block_find_string(&cmpr,"FROM_NAME", from_name, &from_size) != true )
    {
       printf("to name not found \n");
       return false;
        
    }
     

    
    status = false;
    
    
    if( rename(from_name, to_name) == 0)
    {
        status = true;
    }        
    
    temp = malloc(sizeof(MSG_PACK_ELEMENT)*6);
    *msg_pack = temp;
    *msg_pack_number = 6;
    msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
    msg_dict_pack_string(&temp[1],"COMMAND", "FILE_RENAME");
    msg_dict_pack_map(&temp[2],"DATA",2);
    msg_dict_pack_string_dynamic(&temp[3],"TO_NAME", to_name);
   msg_dict_pack_string_dynamic(&temp[4],"FROM_NAME", from_name);
    msg_dict_pack_unsigned_integer(&temp[5],"STATUS", status);

    return true;
 
}    


static int count_directory(char *file_mount )
{ 
  
   struct dirent *de;  // Pointer for directory entry
   DIR *dr;
   int count;
   
   
   count = 0;
  
   dr   = opendir(file_mount);
   if (dr == NULL)  // opendir returns NULL if couldn't open directory
   {
        printf("Could not open current directory \n" );
        return -1;
   }
   
  while ((de = readdir(dr)) != NULL)
  {
            count += 1;
            //printf("name %s\n", de->d_name);
            
           
  }
   closedir(dr); 
   return count;   
    
}    

static int list_directory(int max_count, MSG_PACK_ELEMENT *element, char *file_mount )
{ 
  
   struct dirent *de;  // Pointer for directory entry
   DIR *dr;
   int count;
   char *file_name;
   int file_length;
   
   
   count = 0;
  
   dr   = opendir(file_mount);
   if (dr == NULL)  // opendir returns NULL if couldn't open directory
   {
        printf("Could not open current directory \n" );
       return -1;
   }
   
  while ((de = readdir(dr)) != NULL)
  {
            count += 1;
            //printf("name %s\n", de->d_name);
            file_length = strlen(de->d_name)+4;
            file_name = malloc(file_length);
            memset(file_name,0,file_length);
            strcpy(file_name,de->d_name);
            msg_dict_pack_string_dynamic(element,NULL,file_name);
            element++;
           
            if(count >= max_count)
            {
                break;
            }
           
  }
   closedir(dr); 
   return count;   
    
}  

static bool file_directory(
                          int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    CMP_READ_BLOCK_T cmpr;
   
    MSG_PACK_ELEMENT *temp;
    int pack_elements;
    int file_elements;
    char *file_mount;
    uint32_t size;
    
    file_mount = malloc(FILE_MOUNT_SIZE+1);
    memset(file_mount, 0, FILE_MOUNT_SIZE+1);
    size = FILE_MOUNT_SIZE;
 
    msgpack_block_init(&cmpr,msg_data, msg_data_size);
   

    if( msgpack_block_find_string(&cmpr,"FILE_MOUNT", file_mount, &size) != true )
    {
       printf("directory name not found \n");
       return false;
        
    }
    
  
   
   
   file_elements =  count_directory(file_mount);
   if( file_elements <0){ return false; }
   pack_elements = file_elements +3;
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*pack_elements);
   *msg_pack = temp;
   *msg_pack_number = pack_elements;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "FILE_DIR");
 
   msg_dict_pack_array(&temp[2],"DATA", file_elements);
   list_directory(file_elements, &temp[3],file_mount );
   return true;
}




static bool reboot(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 

                          char* msg_data, 
                          int msg_data_size  )
{
    abort();
    return false;
}
static bool heap_space(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack,  
                          char* msg_data, 
                          int msg_data_size  )
{
    
   MSG_PACK_ELEMENT *temp;
   
   
   
   
   
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*3);
   *msg_pack = temp;
   *msg_pack_number = 3;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "HEAP_SPACE");
   
  
   msg_dict_pack_unsigned_integer(&temp[2],"DATA",esp_get_free_heap_size());
   return true; 
   
    
    
}
static bool list_commands(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack,                                                
                          char* msg_data, 
                          int msg_data_size  )
{
    
   MSG_PACK_ELEMENT *temp;
   int pack_elements;
   int command_elements;
   
   command_elements =  msg_command_command_number() ;
   pack_elements = command_elements +3;
   temp = malloc(sizeof(MSG_PACK_ELEMENT)*pack_elements);
   *msg_pack = temp;
   *msg_pack_number = pack_elements;
   
   msg_dict_pack_string(&temp[0],"TOPIC", "COMMAND_RESPONSE");
   msg_dict_pack_string(&temp[1],"COMMAND", "LIST_COMMANDS");
 
   msg_dict_pack_array(&temp[2],"DATA", command_elements);
   for(int i=0; i<command_elements;i++)
   {
       msg_dict_pack_string(&temp[i+3 ],NULL, msg_command_list_cmd_name(i));
   }
   return true;
}   
   



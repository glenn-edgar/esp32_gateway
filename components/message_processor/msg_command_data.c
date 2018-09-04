#include <esp_types.h>
#include <stdio.h>
#include "cmp.h"
#include "msgpack_utillities.h"
#include "msg_dict_stream.h"
#include "msg_command_cmd_table.h"
#include "msg_command_data.h"



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
    printf("return value %d  \n",return_value);
    if( return_value == true)
    {
        command_function =  msg_command_find_command(command_data,command_size);
        if( command_function != NULL )
        {
            return_value = command_function( msg_pack_number, msg_pack,msgpack_data,msgpack_size);
        }
 
        
    }
    
    return return_value;
}    
    
    
static bool find_command_data(struct cmp_ctx_s *ctx,
                       char **command_data, uint32_t *command_size,
                       char **msgpack_data, uint32_t *msgpack_size)
{
   bool return_value;
   uint32_t map_size;
   int   leave_count = 2;
  
   char *temp_pointer;
   uint32_t   temp_size;
   
   return_value =  cmp_read_map(ctx, &map_size);
   if( return_value != true){ return return_value;}
   
  
   for( int i= 0; i<map_size; i++ )
   {
       return_value =  msgpack_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       check_return(return_value);
       if( ctx_strcmp(COMMAND_FIELD,temp_pointer,temp_size ) == 0)
       {
           
           return_value =  msgpack_scoop_field(ctx,(void **)command_data,command_size );
           check_return(return_value);
           leave_count -=1;
           
       }
       else if( ctx_strcmp(DATA_FIELD,temp_pointer,temp_size ) == 0)
       {
           
           return_value = msgpack_scoop_field(ctx,(void **)msgpack_data,msgpack_size);
           check_return(return_value);
           leave_count -=1;
           
       }
       else
       {
           
          msgpack_skip_field(ctx,1); 
           
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
   msg_command_add_command("REBOOT",reboot) ; 
   msg_command_add_command("HEAP_SPACE",heap_space);  
   msg_command_add_command("LIST_COMMANDS",list_commands);  
    
    
}


static bool file_write(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    return false;
}
static bool file_read(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    return false;
}
static bool file_delete(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    return false;
}
static bool file_directory(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    return false;
}
static bool reboot(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    return false;
}
static bool heap_space(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    
   return false;  
    
}
static bool list_commands(int *msg_pack_number, 
                          MSG_PACK_ELEMENT **msg_pack, 
                          char* msg_data, 
                          int msg_data_size  )
{
    
    
#if 0   
    cmd_number = msg_command_command_number;
    //pack map
    for(int i= 0;i<cmd_number;i++)
    {
       //pack_string()
        
        
    }
#endif
   return false;   
    
    
    
}

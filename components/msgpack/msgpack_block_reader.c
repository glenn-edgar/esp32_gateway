
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "msgpack_utillities.h"
#include "msgpack_block_reader.h"
#include "hexadecimal.h"


static int buffer_size;
static char *buffer;
static char *buffer_ptr;
static int read_index;

#define check_return(return_value )\
    if(return_value != true){ return false; }
 
static void block_reset_buffer(CMP_READ_BLOCK_T *cmpr);
static char *cmp_skip_function(struct cmp_ctx_s *ctx,  void *data, size_t count);
static bool   cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read);
static void dump_buffer(int size);

bool msgpack_block_find_object(CMP_READ_BLOCK_T *cmpr,char *field_name, char **buffer, uint32_t *buffer_size)
{
     bool return_value;
     cmp_ctx_t *ctx;
     char *temp_pointer;
     uint32_t   temp_size;

    
    ctx = &cmpr->ctx;
    block_reset_buffer(cmpr);
    dump_buffer(1000);
    cmp_init(ctx, NULL, cmp_reader_function, cmp_skip_function, NULL);
    while(1)
    { 
       dump_buffer(1000);
       return_value =  msgpack_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       dump_buffer(1000);
       check_return(return_value);
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
           dump_buffer(1000);
           return_value = msgpack_scoop_field(ctx,(void **)buffer,buffer_size);
           
           return return_value;
           
           
       }
           
       msgpack_skip_field(ctx); 
        
        
    }
    return false;
}    

bool msgpack_block_find_string(CMP_READ_BLOCK_T *cmpr,char *field_name, char *string, uint32_t *size )
{
    bool return_value;
    cmp_ctx_t *ctx;
    char *temp_pointer;
    uint32_t   temp_size;
    uint32_t   map_size;

    ctx = &cmpr->ctx;
    block_reset_buffer(cmpr);
    cmp_init(ctx, NULL, cmp_reader_function, cmp_skip_function, NULL);
    
    if( cmp_read_map(ctx, &map_size) != true) {return false;}
   
    
    for(int i = 0;i < map_size; i++)
    {  
       return_value =  msgpack_get_bin_data_ptr(ctx,(void **)&temp_pointer,&temp_size );
       check_return(return_value);
     
       if( ctx_strcmp(field_name,temp_pointer,temp_size ) == true)
       {
           
           memset(string,0,*size);
           
           return_value = cmp_read_str(ctx, string, size);
           
           return return_value;
           
           
       }
       
       msgpack_skip_field(ctx); 
            
    } 
    return false;
}
    
    
static void block_reset_buffer(CMP_READ_BLOCK_T *cmpr)
{
   read_index = 0;
   buffer_ptr =  cmpr->read_buffer;
   buffer     =  cmpr->read_buffer;
   buffer_size = cmpr->read_size;
    
}

static void dump_buffer( int size)
{
    char *hex_buffer;
    int  hex_size;
    if(size >= (buffer_size -read_index) )
    {
        size = buffer_size-read_index;
        
    }  
    hex_size = size *2;
    hex_buffer = malloc(hex_size+1);
    memset(hex_buffer,0,hex_size+1);
    binary_to_hex(buffer_ptr,size, hex_buffer);
    printf("dumping remaining part of buffer size %d \n",size);
    printf(hex_buffer);
    printf("\n\n---\n");
    free(hex_buffer);
    
}  

  
static bool   cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read)
{
    char *return_buffer;
    int i;
    
    return_buffer = (char *)data;
    
    if((buffer_size)< (read_index+number_to_read))
    {
        return false;
    }
    for( i = 0;i<number_to_read;i++)
    {
       *return_buffer++ = *buffer_ptr++;    
        
    }
    read_index += number_to_read;
    return true;
    
}   
    




static char *cmp_skip_function(struct cmp_ctx_s *ctx,  void *data, size_t count)
{
   char *return_value;
   return_value = buffer_ptr;
  
   read_index = read_index +count;
   if( read_index > buffer_size)
   {
       read_index = buffer_size;
   }
   buffer_ptr = buffer + read_index;
  
   return return_value;
    
}







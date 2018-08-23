
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <string.h>
#include "cmp.h"
#include "msg_file_dict.h"


#define FILE_NAME_SIZE 32
#define BUFFER_SIZE 1024

static char *buffer;
static char *current_buffer_ptr;


static int buffer_size;

static FILE *msg_pack_handle;

// file was included because of path problems with include file
static char *spiffs_get_prefix(void)
{
    
  return "/spiffs/";  
    
}

static void reset_buffer(void)
{
    current_buffer_ptr = buffer;
    
    
}

//typedef bool   (*cmp_reader)(struct cmp_ctx_s *ctx, void *data, size_t limit);

bool cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read )
{
    char *return_buffer;
    int i;
    
    return_buffer = (char *)data;
    
    if((buffer+BUFFER_SIZE)<= (current_buffer_ptr+number_to_read))
    {
        return false;
    }
    for( i = 0;i<number_to_read;i++)
    {
       *return_buffer++ = *current_buffer_ptr++;    
        
    }
    return true;
    
}

//typedef size_t (*cmp_writer)(struct cmp_ctx_s *ctx, const void *data,size_t count);
size_t cmp_writer_function(struct cmp_ctx_s *ctx, const void *data, size_t count )
{
    abort(); // no write actions allowed
    
}



//typedef char * (*cmp_skip)(struct cmp_ctx_s *ctx,  void *data,size_t count);
char * cmp_skip_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read )
{

    
    if((buffer+BUFFER_SIZE)<= (current_buffer_ptr+number_to_read))
    {
        return NULL;
    }
    current_buffer_ptr+= number_to_read;
         
        
    
    return current_buffer_ptr;
    
}


void msgpack_load_buffer(char *filename)
{
    
    char *msg_pack_file_name;
    
    msg_pack_file_name = malloc(FILE_NAME_SIZE);
    strcpy(msg_pack_file_name, spiffs_get_prefix());
    strcat( msg_pack_file_name, filename);

    
    printf("file name %s \n",msg_pack_file_name);
    msg_pack_handle = fopen(msg_pack_file_name,"rb");
    free(msg_pack_file_name);
    if(msg_pack_handle == NULL)
    {
        
        abort();
       
    }
    buffer = malloc(BUFFER_SIZE);
    buffer_size = fread(buffer,1,BUFFER_SIZE,msg_pack_handle);
    
    printf("buffer_size %d \n",buffer_size);
    
    
}

bool msgpack_find_field( char *field, char *result, int max_size )
{
    cmp_ctx_t cmp;
    unsigned map_size;
    unsigned i;
    bool return_value;
    
    unsigned string_size;
    
    
    return_value = false;   
    reset_buffer();
    cmp_init( &cmp, buffer, cmp_reader_function, cmp_skip_function, cmp_writer_function);
    if( cmp_read_map(&cmp, &map_size ) == false )
    {
        abort();
    }
    
    for( i = 0; i < map_size; i++)
    {
        string_size = max_size;
        if(cmp_read_str(&cmp, result, &string_size) == false)
        {
            abort();
        }

        
        if( strcmp(field,result) == 0 )
        {
           string_size = max_size;
           if(cmp_read_str(&cmp, result, &string_size) == false)
           {
              abort();
           }
          
                
           return true;           
            
            
        }
        else
        {
           string_size = max_size;
           if(cmp_read_str(&cmp, result, &string_size) == false)
           {
              abort();
           }
 
        }

   }
   abort(); // did not find any results     
   return return_value ; 
        
        
   
    
}



void msgpack_close_buffer(void)
{
    
    free(buffer);
    
    fclose(msg_pack_handle);
    
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <esp_types.h>
#include "cmp.h"
#include "msgpack_utillities.h"



static cmp_ctx_t cmp;
static int buffer_size;
static char *buffer;
static char *buffer_ptr;
static int read_index;


static bool   cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read);
static char *cmp_skip_function(struct cmp_ctx_s *ctx,  void *data, size_t count);
bool analyize_current_object(cmp_ctx_t *ctx, cmp_object_t *obj,uint32_t *skip_num, uint32_t *size); 


void reset_buffer(void)
{
   read_index = 0;
   buffer_ptr = buffer;
  
}
cmp_ctx_t *msppack_return_handle( void)
{
    return &cmp;
}

bool ctx_strcmp(char *normal_string,char *binary_string,int binary_size )
{
    int string_len;
    
    string_len = strlen(normal_string);
    if( string_len != binary_size){ return false;}
    for(int i=0;i<string_len;i++)
    {
        if(*normal_string++ != *binary_string++)
        {
            return false;
        }
        
    }
    return true;
    
}

cmp_ctx_t * msgpack_dict_scan_init( char *buf, int buf_size )
{
    buffer = buf;
    buffer_size = buf_size;
    reset_buffer();
    cmp_init( &cmp, buffer, cmp_reader_function, cmp_skip_function, NULL );
    return &cmp;


}





bool msgpack_get_bin_data_ptr(cmp_ctx_t *ctx ,void **data, uint32_t *size )
{
    bool return_value;
    
    return_value =  cmp_read_bin_size(ctx, size);
    if(return_value == true)
    {
        *data =  cmp_skip_function(NULL,NULL, *size);
        
    }        
    return return_value;
    
    
    
    
}


bool msgpack_skip_field(cmp_ctx_t *ctx, uint32_t number_to_skip)
{
    bool return_value;
    cmp_object_t obj;
    uint32_t          size;
    
    return_value = true;
    while((number_to_skip > 0) && (return_value == true))
    {
        number_to_skip -= 1;
        return_value = analyize_current_object(ctx, &obj, &number_to_skip, &size);
        if( return_value == true)
        {
          return_value = cmp_skip_function( NULL,NULL, size ); 
      
        }
    }
    return return_value;
    
}

bool  msgpack_scoop_field(cmp_ctx_t  *ctx, void **data, uint32_t *size)
{
    bool return_value;
    cmp_object_t obj;
   
    uint32_t    number_to_skip = 1;
    char        *reference_point;
    
    reference_point = buffer_ptr;
    return_value = true;
    while((number_to_skip > 0) && (return_value == true))
    {
        number_to_skip -= 1;
        return_value = analyize_current_object(ctx, &obj, &number_to_skip, size);
        if( return_value == true)
        {
          return_value = cmp_skip_function( NULL,NULL, *size );   
        }
    }
    *size = buffer_ptr-reference_point;
    *data = reference_point;
    return return_value;
    
    
}



static bool   cmp_reader_function(struct cmp_ctx_s *ctx, void *data, size_t number_to_read)
{
    char *return_buffer;
    int i;
    
    return_buffer = (char *)data;
    
    if((buffer_size)<= (read_index+number_to_read))
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
   read_index = read_index +count;
   if( read_index > buffer_size)
   {
       read_index = buffer_size;
   }
   return buffer_ptr;
    
}



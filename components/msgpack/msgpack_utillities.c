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

static char cmp_peak(void);

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
        *data =  ctx->skip(NULL,NULL, *size);
        
    } 
    return return_value;
    
    
    
    
}


bool msgpack_skip_field(cmp_ctx_t *ctx)
{
    bool return_value;
    cmp_object_t obj;
    uint32_t          size;
    uint32_t          number_to_skip = 1;
    
    return_value = true;
    
    while((number_to_skip > 0) && (return_value == true))
    {
        
        number_to_skip -= 1;
        return_value = analyize_current_object(ctx, &obj, &number_to_skip, &size);
        
        if( return_value == true)
        {
            ctx->skip( NULL,NULL, size ); 
      
        }
    }
    
    return return_value;
    
}

bool  msgpack_scoop_field(cmp_ctx_t  *ctx, void **data, uint32_t *size)
{
    bool return_value;
    cmp_object_t obj;
   
    uint32_t    number_to_skip = 1;
   
    uint32_t    temp_size;
    uint32_t    temp_data_size =0;
    
    
    number_to_skip -= 1;
    *data = buffer_ptr;
    
    return_value = analyize_current_object(ctx, &obj, &number_to_skip, &temp_size);
    
    if( return_value == true)
    {
          ctx->skip( NULL,NULL,temp_size );   
    }
    if(return_value == false){return false; }
    return_value = true;
    while((number_to_skip > 0) && (return_value == true))
    {
        number_to_skip -= 1;
        return_value = analyize_current_object(ctx, &obj, &number_to_skip, &temp_size);
        temp_data_size += temp_size;
        if( return_value == true)
        {
            ctx->skip( NULL,NULL, temp_size );   
        }
        else
        {
            return false;
        }
        
    }
    
    
   *size = buffer_ptr - (char *)*data;
    printf("size is %d temp_data_size %d\n",*size,temp_data_size);
    return return_value;
    
    
}

static char cmp_peak(void)
{
    return *buffer_ptr;
    
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
   char *return_value;
   return_value = buffer_ptr;
   read_index = read_index +count;
   
   if( read_index > buffer_size)
   {
       abort(); //read_index = buffer_size;
   }
   buffer_ptr = buffer + read_index;
   
   return return_value;
    
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmp.h"
#include "msg_dict_stream.h"



static char *current_buffer = NULL;
static int buffer_size = 0;
static char *buffer;

static void allocate_buffer( int number, MSG_PACK_ELEMENT *msg_pack );

static bool reader(struct cmp_ctx_s *ctx, void *data, size_t number_to_read );
static size_t writer(struct cmp_ctx_s *ctx, const void *data, size_t count );
static char * skipper(struct cmp_ctx_s *ctx, void *data, size_t number_to_read );



char * msg_dict_stream(  int *buff_size, int number, MSG_PACK_ELEMENT *msg_pack)

{
  cmp_ctx_t ctx;
  
  allocate_buffer( number, msg_pack );
  *buff_size = buffer_size;
  cmp_init(&ctx, buffer,reader, skipper, writer);    
  cmp_write_map(&ctx,number);
  for(int i=0; i<number;i++)
  {
      
    
    cmp_write_str(&ctx,msg_pack->field_name ,strlen(msg_pack->field_name) );
    switch(msg_pack->type)
    {
        
        case STR_TYPE: // string data
          cmp_write_str(&ctx,msg_pack->data.string ,msg_pack->size );
          break;
          
          
        case INT_TYPE: // integer data
          cmp_write_integer(&ctx , msg_pack->data.integer);
           break;
           
        case UINT_TYPE: // integer data
           cmp_write_uinteger(&ctx, msg_pack->data.integer);
           break;


           
        case FLOAT_TYPE: // float data
          cmp_write_decimal(&ctx, msg_pack->data.real);
           break;
        
       case BIN_TYPE:  //binary data
          cmp_write_bin(&ctx, msg_pack->data.string ,msg_pack->size );
           break; 
       
       default:
       while(1){printf("should not be here \n");}
         abort();
    }
    msg_pack++;
    
           
  }
  
  return buffer;
}  
    
    

static void allocate_buffer( int number, MSG_PACK_ELEMENT *msg_pack )
{
    int size =0;
   
    size = 10; //allocate map
    for(int i=0;i<number;i++)
    {
      switch(msg_pack->type)
      {
        case STR_TYPE: // string data
          size += (msg_pack->size + 6);
          break;
          
          
        case INT_TYPE: // integer data
          size += 14;
           break;
           
        case UINT_TYPE: // integer data
           size += 14;
           break;


           
        case FLOAT_TYPE: // float data
          size += 14;
           break;
        
       case BIN_TYPE:  //binary data
           size += (msg_pack->size + 6);
           break; 
       
        
      }
      msg_pack++;
    }
    
    buffer= malloc(size);
    if(buffer == NULL)
    {
        abort();
    }
    buffer_size = size;
    current_buffer = buffer;
    
    
    
}

static bool reader(struct cmp_ctx_s *ctx, void *data, size_t number_to_read )
{
   abort(); // should not happen
    
}

//typedef size_t (*cmp_writer)(struct cmp_ctx_s *ctx, const void *data,size_t count);
static size_t writer(struct cmp_ctx_s *ctx, const void *data, size_t count )
{

    if((current_buffer+count)>= buffer +buffer_size)
    {
       abort(); 
    }
    memcpy(current_buffer,data,count);
    current_buffer += count;
    
    return count;
    
}



//typedef char * (*cmp_skip)(struct cmp_ctx_s *ctx,  void *data,size_t count);
static char * skipper(struct cmp_ctx_s *ctx, void *data, size_t number_to_read )
{

    
    abort(); // should not happen
    
}

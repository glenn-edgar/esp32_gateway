
#ifndef _MSGPACK_BLOCK_READER_H_
#define _MSGPACK_BLOCK_READER_H_

#include "cmp.h"
#include <esp_types.h>

typedef struct
{
    
   cmp_ctx_t ctx;
   char   *read_buffer;
   char   *current_pointer;   
   int    read_size;    
    
}CMP_READ_BLOCK_T;


static inline void msgpack_block_init(CMP_READ_BLOCK_T *cmpr, char *read_buffer, int read_size)
{
   cmpr->read_buffer = read_buffer;
   cmpr->read_size = read_size;

    
}


bool msgpack_block_find_string(CMP_READ_BLOCK_T *cmpr,char *field_name, char *string, uint32_t *size );





bool msgpack_block_find_object(CMP_READ_BLOCK_T *cmpr,char *field_name, char **buffer, uint32_t *buffer_size);




#endif
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
static bool analyize_current_object(cmp_ctx_t *ctx, cmp_object_t *obj,uint32_t *skip_num, uint32_t *size); 


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

#if 0
static bool analyize_current_object(cmp_ctx_t *ctx, cmp_object_t *obj,int *skip_num, int *size) 
{
  uint8_t type_marker = 0;
  *size = 0;
  if (!read_type_marker(ctx, &type_marker))
    return false;

  if (type_marker <= 0x7F) {
    obj->type = CMP_TYPE_POSITIVE_FIXNUM;
    obj->as.u8 = type_marker;
  }
  else if (type_marker <= 0x8F) {
    obj->type = CMP_TYPE_FIXMAP;
    obj->as.map_size = type_marker & FIXMAP_SIZE;
    *size = obj->as.map_size;
    skip_number += *size;
  }
  else if (type_marker <= 0x9F) {
    obj->type = CMP_TYPE_FIXARRAY;
    obj->as.array_size = type_marker & FIXARRAY_SIZE;
    
    skip_number +=  *size;
  }
  else if (type_marker <= 0xBF) {
    obj->type = CMP_TYPE_FIXSTR;
    obj->as.str_size = type_marker & FIXSTR_SIZE;
    *size = obj->as.str_size;
  }
  else if (type_marker == NIL_MARKER) {
    obj->type = CMP_TYPE_NIL;
    obj->as.u8 = 0;
    
  }
  else if (type_marker == FALSE_MARKER) {
    obj->type = CMP_TYPE_BOOLEAN;
    obj->as.boolean = false;
  }
  else if (type_marker == TRUE_MARKER) {
    obj->type = CMP_TYPE_BOOLEAN;
    obj->as.boolean = true;
  }
  else if (type_marker == BIN8_MARKER) {
    obj->type = CMP_TYPE_BIN8;
    if (!ctx->read(ctx, &obj->as.u8, sizeof(uint8_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    obj->as.bin_size = obj->as.u8;
    *size = obj->as.bin_size;
  }
  else if (type_marker == BIN16_MARKER) {
    obj->type = CMP_TYPE_BIN16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    obj->as.bin_size = be16(obj->as.u16);
    *size = obj->as.bin_size;
  }
  else if (type_marker == BIN32_MARKER) {
    obj->type = CMP_TYPE_BIN32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    obj->as.bin_size = be32(obj->as.u32);
    *size = obj->as.bin_size;
  }
  else if (type_marker == EXT8_MARKER) {
    uint8_t ext_size;
    int8_t ext_type;

    obj->type = CMP_TYPE_EXT8;
    if (!ctx->read(ctx, &ext_size, sizeof(uint8_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    if (!ctx->read(ctx, &ext_type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = ext_size;
    obj->as.ext.type = ext_type;
    *size = obj->as.ext.size
  }
  else if (type_marker == EXT16_MARKER) {
    int8_t ext_type;
    uint16_t ext_size;

    obj->type = CMP_TYPE_EXT16;
    if (!ctx->read(ctx, &ext_size, sizeof(uint16_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    if (!ctx->read(ctx, &ext_type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = be16(ext_size);
    obj->as.ext.type = ext_type;
    *size = obj->as.et.size;
  }
  else if (type_marker == EXT32_MARKER) {
    int8_t ext_type;
    uint32_t ext_size;

    obj->type = CMP_TYPE_EXT32;
    if (!ctx->read(ctx, &ext_size, sizeof(uint32_t))) {
      ctx->error = LENGTH_READING_ERROR;
      return false;
    }
    if (!ctx->read(ctx, &ext_type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = be32(ext_size);
    obj->as.ext.type = ext_type;
    *size = obj->as.ext.size;
  }
  else if (type_marker == FLOAT_MARKER) {
    char bytes[sizeof(float)];

    obj->type = CMP_TYPE_FLOAT;
    if (!ctx->read(ctx, bytes, sizeof(float))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.flt = decode_befloat(bytes);
  }
  else if (type_marker == DOUBLE_MARKER) {
    char bytes[sizeof(double)];

    obj->type = CMP_TYPE_DOUBLE;
    if (!ctx->read(ctx, bytes, sizeof(double))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.dbl = decode_bedouble(bytes);
  }
  else if (type_marker == U8_MARKER) {
    obj->type = CMP_TYPE_UINT8;
    if (!ctx->read(ctx, &obj->as.u8, sizeof(uint8_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
  }
  else if (type_marker == U16_MARKER) {
    obj->type = CMP_TYPE_UINT16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.u16 = be16(obj->as.u16);
  }
  else if (type_marker == U32_MARKER) {
    obj->type = CMP_TYPE_UINT32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.u32 = be32(obj->as.u32);
  }
  else if (type_marker == U64_MARKER) {
    obj->type = CMP_TYPE_UINT64;
    if (!ctx->read(ctx, &obj->as.u64, sizeof(uint64_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.u64 = be64(obj->as.u64);
  }
  else if (type_marker == S8_MARKER) {
    obj->type = CMP_TYPE_SINT8;
    if (!ctx->read(ctx, &obj->as.s8, sizeof(int8_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
  }
  else if (type_marker == S16_MARKER) {
    obj->type = CMP_TYPE_SINT16;
    if (!ctx->read(ctx, &obj->as.s16, sizeof(int16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.s16 = be16(obj->as.s16);
  }
  else if (type_marker == S32_MARKER) {
    obj->type = CMP_TYPE_SINT32;
    if (!ctx->read(ctx, &obj->as.s32, sizeof(int32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.s32 = be32(obj->as.s32);
  }
  else if (type_marker == S64_MARKER) {
    obj->type = CMP_TYPE_SINT64;
    if (!ctx->read(ctx, &obj->as.s64, sizeof(int64_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.s64 = be64(obj->as.s64);
  }
  else if (type_marker == FIXEXT1_MARKER) {
    obj->type = CMP_TYPE_FIXEXT1;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 1;
  }
  else if (type_marker == FIXEXT2_MARKER) {
    obj->type = CMP_TYPE_FIXEXT2;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 2;
    *size = 2
  }
  else if (type_marker == FIXEXT4_MARKER) {
    obj->type = CMP_TYPE_FIXEXT4;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 4;
    *size = 4;
  }
  else if (type_marker == FIXEXT8_MARKER) {
    obj->type = CMP_TYPE_FIXEXT8;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 8;
    *size = 8;
  }
  else if (type_marker == FIXEXT16_MARKER) {
    obj->type = CMP_TYPE_FIXEXT16;
    if (!ctx->read(ctx, &obj->as.ext.type, sizeof(int8_t))) {
      ctx->error = EXT_TYPE_READING_ERROR;
      return false;
    }
    obj->as.ext.size = 16;
    *size = 16;
  }
  else if (type_marker == STR8_MARKER) {
    obj->type = CMP_TYPE_STR8;
    if (!ctx->read(ctx, &obj->as.u8, sizeof(uint8_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.str_size = obj->as.u8;
    *size = obj->as.u8;
  }
  else if (type_marker == STR16_MARKER) {
    obj->type = CMP_TYPE_STR16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.str_size = be16(obj->as.u16);
    *size = obj->as.str_size;
  }
  else if (type_marker == STR32_MARKER) {
    obj->type = CMP_TYPE_STR32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.str_size = be32(obj->as.u32);
    *size = obj->as.str_size;
  }
  else if (type_marker == ARRAY16_MARKER) {
    obj->type = CMP_TYPE_ARRAY16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.array_size = be16(obj->as.u16);
    *skip_num += obj->as.str_size;
  }
  else if (type_marker == ARRAY32_MARKER) {
    obj->type = CMP_TYPE_ARRAY32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.array_size = be32(obj->as.u32);
    
    *skip_num += *size;
  }
  else if (type_marker == MAP16_MARKER) {
    obj->type = CMP_TYPE_MAP16;
    if (!ctx->read(ctx, &obj->as.u16, sizeof(uint16_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.map_size = be16(obj->as.u16);
    *skip_num += *size;
  }
  else if (type_marker == MAP32_MARKER) {
    obj->type = CMP_TYPE_MAP32;
    if (!ctx->read(ctx, &obj->as.u32, sizeof(uint32_t))) {
      ctx->error = DATA_READING_ERROR;
      return false;
    }
    obj->as.map_size = be32(obj->as.u32);
    *skip_num += *size;
  }
  else if (type_marker >= NEGATIVE_FIXNUM_MARKER) {
    obj->type = CMP_TYPE_NEGATIVE_FIXNUM;
    obj->as.s8 = type_marker;
  }
  else {
    ctx->error = INVALID_TYPE_ERROR;
    return false;
  }

  return true;
}
#endif
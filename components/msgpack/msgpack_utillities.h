#ifndef _MSGPACK_UTILITIES_H_
#define _MSGPACK_UTILITIES_H_

#include "cmp.h"

void reset_buffer(void);

cmp_ctx_t * msgpack_dict_scan_init( char *buf, int buf_size );

cmp_ctx_t *msppack_return_handle( void);



bool ctx_strcmp(char *normal_string,char *binary_string,int binary_size );



bool msgpack_get_bin_data_ptr(cmp_ctx_t *ctx ,void **data, uint32_t *size );

bool msgpack_skip_field(cmp_ctx_t *ctx, uint32_t number_to_skip);

bool  msgpack_scoop_field(cmp_ctx_t *ctx, void **data, uint32_t *size);

#endif
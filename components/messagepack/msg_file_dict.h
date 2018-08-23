

#ifndef _MSG_FILE_DICT_H_
#define _MSG_FILE_DICT_H_

void msgpack_load_buffer(char *file_name);

bool msgpack_find_field( char *field, char *result, int max_size );



void msgpack_close_buffer(void);


#endif

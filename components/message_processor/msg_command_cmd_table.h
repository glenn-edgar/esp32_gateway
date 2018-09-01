
#ifndef _MSG_COMMAND_CMD_TABLE_H_
#define _MSG_COMMAND_CMD_TABLE_H_

#include "cmp.h"

#define COMMAND_TABLE_NUMBER 64


typedef bool (*PROCESS_COMMAND_T)( char* msg_data, int msg_data_size  );
 
void initialize_command_table( void );

void msg_command_add_command(char *command_name,PROCESS_COMMAND_T process_command);

PROCESS_COMMAND_T msg_command_find_command(char *command_name, uint32_t command_size);

int msg_command_command_number(void);
#endif
    
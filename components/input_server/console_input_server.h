
#ifndef _CONSOLE_INPUT_SERVER_H_
#define _CONSOLE_INPUT_SERVER_H_

typedef void (*COMMAND_PROCESS)(int length, char *data);


bool console_input_init(void);
bool console_input_attach_command(char *command,COMMAND_PROCESS command_function);





#endif
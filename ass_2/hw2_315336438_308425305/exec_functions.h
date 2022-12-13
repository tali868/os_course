#ifndef EXECC_H 
#define EXECC_H 

#include <pthread.h>
#include "structs.h"

void* read_and_execute(void *input);
void run_command_line(char* command_line, char** count_files);
void execute_dispatcher(char *command, pthread_t* threads, Queue *q, int** is_busy);

#endif
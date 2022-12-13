#ifndef PARSING_H 
#define PARSING_H

#include <pthread.h>

bool is_worker(char *line);
bool is_dispatcher(char *line);
char *mystrtok(char *str, char *delimiters);

#endif
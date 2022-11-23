#include "shell.h"

void trim_sides(char** user_input);
void parse_backround(Instruction *instruction, char* user_input);
void parse_user_input(Instruction *instruction, char* user_input);
void parse_input(Instruction *instruction, char** user_input);

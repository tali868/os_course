#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsing.h"

void trim_sides(char** user_input)
{
    int len = strlen(*user_input);
    int word_start = 0;
    int word_end = len - 1;
    char* clean_instruction;
    size_t to_be_copied;
    while(isspace((*(*user_input + word_start)))) ++word_start;
    while(isspace(*(*user_input + word_end))) --word_end;
    *(*user_input + word_end + 1) = 0;
    *user_input = *user_input + word_start;
}

void parse_backround(Instruction *instruction, char* user_input)
{
    int amp_ix = NULL;
    int len = strlen(user_input) - 1;
    int i;
    instruction->is_backround = false;
    for (i = len; i > 0; i-=1) {
        if (user_input[i] == '&')
        {
            amp_ix = i;
            strncpy(instruction->raw_instruction, user_input, i);
        }
    }
    if (amp_ix != NULL)  // found &
    {
        trim_sides(&(instruction->raw_instruction));
        instruction->operation = INST;
        instruction->is_backround = true;
    }
}

void parse_user_input(Instruction *instruction, char* user_input)
{
    int len = strlen(user_input) - 1;  // to skip "\0"
    int word_start = 0;
    char* op;
    char* token;
    bool found_known = false;

    strcpy(instruction->raw_instruction, user_input);
    op = strtok(user_input, " ");
    if (strcmp(op, "jobs") == 0)
    {
        instruction->operation = JOBS;
        found_known = true;
        return;
    }
    if (strcmp(op, "exit") == 0)
    {
        instruction->operation == EXIT;
        found_known = true;
        return;
    }
    if (strcmp(op, "cd") == 0)
    {
        instruction->operation = CD;
        found_known = true;
        token = strtok(NULL, "\n");
        strcpy(instruction->directory, token);
    }
    if (found_known == false)
    {
        instruction->operation = INST;
        return;
    }
}

void parse_input(Instruction *instruction, char** user_input)
{
    parse_backround(instruction, *user_input);
    if (instruction->is_backround == true) return;

    parse_user_input(instruction, *user_input);
}
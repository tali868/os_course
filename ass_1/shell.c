#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

void trim_sides(char** user_input)
{
    int len = strlen(*user_input) + 1;
    int word_start = 0;
    int word_end = len - 1;
    char* clean_instruction;
    size_t to_be_copied;
    while(isspace(*(*(user_input) + word_start))) ++word_start;
    clean_instruction = strtok(*user_input, "\n");
    clean_instruction = strtok(clean_instruction, "\t");
    clean_instruction = clean_instruction + word_start;
    strcpy(*user_input, clean_instruction);
}

void parse_backround(Instruction *instruction, char* user_input)
{
    int amp_ix = NULL;
    int len = strlen(user_input) - 1;
    int i;
    instruction->backround = false;
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
        strcpy(instruction->operation, "inst");
        instruction->backround = true;
    }
}

void parse_user_input(Instruction *instruction, char* user_input)
{
    int len = strlen(user_input) - 1;  // to skip "\0"
    int word_start = 0;
    char* op;
    char* token;

    strcpy(instruction->raw_instruction, user_input);
    op = strtok(user_input, " ");
    if (strcmp(op, "cd") != 0 && strcmp(op, "jobs") != 0 && strcmp(op, "exit") != 0)
    {
        strcpy(instruction->operation, "inst");
        return;
    }
    else
    {
        strcpy(instruction->operation, op);
    }
    if (!strcmp(op, "cd"))
    {
        token = strtok(NULL, "\n");
        strcpy(instruction->directory, token);
    }
}

void parse_input(Instruction *instruction, char** user_input)
{
    char curr_word [4];
    int len = strlen(*user_input) + 1;
    int word_start = 0;
    parse_backround(instruction, *user_input);
    if (instruction->backround == true) return;

    parse_user_input(instruction, *user_input);
}

void execute_input(Instruction *instruction)
{
    if (strcmp(instruction->operation, "cd") == 0)
    {
        // TODO: TALI
    }
    else if (strcmp(instruction->operation, "jobs") == 0)
    {
        // TODO: TALI
    }
    else {  // exit is handeled prior to this
    }
}


// TODO: TALI move all above functions to "imput parsing" files


int main(int argc, char *argv[])
{
    Instruction curr_instruction;
    size_t buf_size = MAX_LINE_LENGHT;
    char *input_buffer;
    struct JobNode job;  // TODO - RAZ allocate space when running a new job for raw_instruction

    input_buffer = (char *)malloc(buf_size * sizeof(char));
    curr_instruction.raw_instruction = (char *)malloc(sizeof(char) * MAX_LINE_LENGHT);
    curr_instruction.operation = (char *)malloc(sizeof(char) * MAX_LINE_LENGHT);
    curr_instruction.directory = (char *)malloc(sizeof(char) * MAX_LINE_LENGHT);

    if (input_buffer == NULL)
    {
        perror("Unable to allocate buffer for input");
        exit(1);
    }

    while (1)
    {
        printf("hw1shell$ ");  // TODO: cd will change the directory?
        getline(&input_buffer, &buf_size, stdin);
        if (!strcmp(input_buffer, "\n"))
        {
            continue;
        }
        trim_sides(&input_buffer);
        parse_input(&curr_instruction, &input_buffer);
        if (strcmp(curr_instruction.operation, "exit") == 0) break;
        execute_input(&curr_instruction);

    }

    // TODO: TALI free jobs linked list
    free(curr_instruction.raw_instruction);
    free(curr_instruction.directory);
    free(curr_instruction.operation);
    free(input_buffer);
    
}
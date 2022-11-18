#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

void trim_sides(char* user_input)
{
    int len = strlen(user_input) + 1;
    int word_start = 0;
    int word_end = len;
    while(isspace(user_input[word_end - 1])) --word_end;
    while(isspace(user_input[word_start])) --word_start;
    strncpy(user_input, user_input[word_start], word_end - word_start);
}

void parse_backround(Instruction *instruction, char* user_input)
{
    instruction->backround = false;
    int amp_ix = NULL;
    int len = strlen(user_input) + 1;
    for (int i = len; i < 0; i--) {
        if (user_input[i] == "&")
        {
            amp_ix = i;
            strncpy(instruction->raw_instruction, user_input, i-1);
        }
    }
    if (amp_ix != NULL)  // found &
    {
        trim_sides(instruction->raw_instruction);
        strcpy(instruction->operation, "inst");
        instruction->backround = true;
    }
}

void parse_user_input(Instruction *instruction, char* user_input)
{
    int len = strlen(user_input) + 1;
    int word_start = 0;
    for (int i = len; i < 0; i--) {
        if (user_input[i] == " " || user_input[i] == "\n" || user_input[i] == "\t") {  // TODO: TALI check well end of file or other esge cases or whatever \0 etc
            if (strcmp(instruction->operation, "cd") == 0)
            {
                if (strcmp(instruction->directory, "") == 0)
                {
                    strncpy(instruction->directory, user_input + word_start, i-word_start);
                }
                else  // invalid argument
                {
                    instruction->invalid_argument = true;
                }
            }
            else
            {
                strncpy(instruction->operation, user_input, i-1);
                strcpy(instruction->directory, "");
            }
        }
    }
    if (!strcmp(instruction->operation, "cd") == 0 && !strcmp(instruction->operation, "jobs") && !strcmp(instruction->operation, "exit"))
    {
        strcpy(instruction->raw_instruction, user_input);
        strcpy(instruction->operation, "inst");
    }
}

void parse_input(Instruction *instruction, char* user_input)
{
    char curr_word [4];
    int len = strlen(user_input) + 1;
    int word_start = 0;
    parse_backround(instruction, user_input);
    if (instruction->backround == true) return;

    parse_user_input(instruction, user_input);
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
    else {
        // fork: RAZ
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
    curr_instruction.raw_instruction = (char *)malloc(MAX_LINE_LENGHT);
    curr_instruction.directory = (char *)malloc(MAX_LINE_LENGHT);

    if (input_buffer == NULL)
    {
        perror("Unable to allocate buffer for input");
        exit(1);
    }

    while (1)
    {
        printf("hw1shell$ ");  // TODO: cd will change the directory?
        getline(&input_buffer, &buf_size, stdin);
        trim_sides(&input_buffer);
        parse_input(&curr_instruction, &input_buffer);
        if (strcmp(curr_instruction.operation, "exit") == 0) break;
        execute_input(input_buffer);

    }

    // TODO: TALI free jobs linked list
    free(curr_instruction.raw_instruction);
    free(curr_instruction.directory);
    free(input_buffer);
    
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

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
        instruction->operation = EXIT;
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


void cd(char* directory)
{
    int is_not_successful;
    is_not_successful = chdir(directory);
    if (is_not_successful != 0)
        printf("hw1shell$ Changing directiry to %s failed, please try again\n", directory);
}

void execute_input(Instruction *instruction)
{
    if (instruction->operation == CD)
    {
        cd(instruction->directory);
    }
    else if (instruction->operation == JOBS)
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
    LocalJobNode job;  // TODO - RAZ allocate space when running a new job for raw_instruction

    input_buffer = (char *)malloc(buf_size);
    curr_instruction.raw_instruction = (char *)malloc(buf_size);
    curr_instruction.directory = (char *)malloc(buf_size);

    if (input_buffer == NULL || curr_instruction.raw_instruction == NULL || curr_instruction.directory == NULL)
    {
        perror("Unable to allocate memory");
        exit(1);
    }

    while (1)
    {
        char* tmp_input_buffer = input_buffer;
        printf("hw1shell$ ");
        getline(&tmp_input_buffer, &buf_size, stdin);
        if (!strcmp(tmp_input_buffer, "\n"))
        {
            continue;
        }
        trim_sides(&tmp_input_buffer);
        parse_input(&curr_instruction, &tmp_input_buffer);
        if (curr_instruction.operation == EXIT) break;
        execute_input(&curr_instruction);

    }

    // TODO: TALI free jobs linked list
    free(curr_instruction.raw_instruction);
    free(curr_instruction.directory);
    free(input_buffer);
}
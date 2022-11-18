#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"


void handle_exit()
{
    // kill all processes, free all allocater memory and exit
}

void parse_input(Instruction instruction, char* user_input)
{
    // TODO: parse input according to spaces etx - exit, cd and jobs
}

void execute_input(char* user_input)
{
    Instruction curr_instruction;
    parse_input(*curr_instruction, user_input);
    if (strcmp(curr_instruction.operation, "exit") == 0)
        {
            handle_exit();
            printf("hw1shell$ exiting...\n");
            exit(0);
        }
    if (strcmp(curr_instruction.operation, "cd") == 0)
    {
        // TODO
    }
    if (strcmp(curr_instruction.operation, "cd") == 0)
    {
        
    }

}



int main(int argc, char *argv[])
{
    char user_input[MAX_LINE_LENGHT];
    while (1)
    {
        printf("hw1shell$ ");  // TODO: cd will change the directory! note to future
        scanf("%s", &user_input);
        execute_input(user_input);
        // printf("User input - %s", user_input);
    }
    
}
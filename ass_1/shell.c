#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include "shell.h"

void check_alive_jobs(JobNode jobs[4])
{
    int status;
    int curr_status;
    JobNode curr_job;

    for (int i = 0; i < 4; i+=1) {
        curr_job = jobs[i];
        if (curr_job.is_active == true)
        {
            curr_status = waitpid(curr_job.pid, &status, WNOHANG);
            if (curr_status == 0)
            {
                jobs[i].is_active = false;
            }
        }
    }
}

void kill_all_jobs(JobNode jobs[4])
{
    JobNode curr_job;
    check_alive_jobs(jobs);
    for (int i = 0; i < 4; i+=1) {
        curr_job = jobs[i];
        if (curr_job.is_active == true)
        {
            // kill(pid)
        }
    }
}


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

void jobs_func(JobNode jobs[4])
{
    check_alive_jobs(jobs);
    for (int i = 0; i < 4; i+=1) {
        if (jobs[i].is_active == true)
        {
            printf("%ld\t%s", jobs[i].pid, jobs[i].raw_instruction);
        }
    }
}


void cd(char* directory)
{
    int is_not_successful;
    is_not_successful = chdir(directory);
    if (is_not_successful != 0)
        printf("hw1shell$ Changing directiry to %s failed, please try again\n", directory);
}

void execute_input(Instruction *instruction, JobNode jobs[4])
{
    if (instruction->operation == CD)
    {
        cd(instruction->directory);
    }
    else if (instruction->operation == JOBS)
    {
        jobs_func(jobs);
    }
    else {  // exit is handeled prior to this
        jobs[0].pid = 3424;
        jobs[0].is_active = true;
    }
}
// TODO: TALI move all above functions to "imput parsing" files


bool allocate_jobs(JobNode* jobs, int buf_size)
{
    jobs[0].raw_instruction = (char *)malloc(buf_size);
    jobs[1].raw_instruction = (char *)malloc(buf_size);
    jobs[2].raw_instruction = (char *)malloc(buf_size);
    jobs[3].raw_instruction = (char *)malloc(buf_size);
    if (jobs[0].raw_instruction == NULL || jobs[1].raw_instruction == NULL || jobs[2].raw_instruction == NULL || jobs[3].raw_instruction == NULL)
    {
        return false;
    }
    return true;
}

void free_jobs(JobNode* jobs)
{
    free(jobs[0].raw_instruction);
    free(jobs[1].raw_instruction);
    free(jobs[2].raw_instruction);
    free(jobs[3].raw_instruction);
}

int main(int argc, char *argv[])
{
    Instruction curr_instruction;
    size_t buf_size = MAX_LINE_LENGHT;
    char *input_buffer;
    int job_allocation_status;  // TODO - RAZ allocate space when running a new job for raw_instruction
    JobNode jobs[4];

    input_buffer = (char *)malloc(buf_size);
    curr_instruction.raw_instruction = (char *)malloc(buf_size);
    curr_instruction.directory = (char *)malloc(buf_size);
    job_allocation_status = allocate_jobs(jobs, buf_size);

    if (input_buffer == NULL || curr_instruction.raw_instruction == NULL || curr_instruction.directory == NULL || job_allocation_status == false)
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
        execute_input(&curr_instruction, jobs);
    }

    kill_all_jobs(&jobs);
    free_jobs(jobs);
    free(curr_instruction.raw_instruction);
    free(curr_instruction.directory);
    free(input_buffer);
}
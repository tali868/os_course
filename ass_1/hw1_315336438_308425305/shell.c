#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include "shell.h"

int check_alive_jobs(JobNode jobs[4])
{
    int status;
    int curr_status;
    JobNode curr_job;
    int alive_jobs = 0;
    pid_t curr_pid;

    for (int i = 0; i < 4; i+=1) {
        curr_job = jobs[i];
        if (curr_job.is_active == true)
        {
            alive_jobs++;
            curr_pid = curr_job.pid;
            curr_status = waitpid(curr_pid, &status, WNOHANG);
            if (curr_status == -1)
            {
                printf("hw1shell: %s failed, errno is %d\n", "waitpid", errno);
                exit(1);
            }
            if (curr_status != 0)
            {
                printf("hw1shell: pid %ld finished\n", jobs[i].pid);
                jobs[i].is_active = false;
                alive_jobs--;
            }
        }
    }
    return alive_jobs;
}

void kill_all_jobs(JobNode jobs[4])
{
    JobNode curr_job;   
    int kill_status;
    pid_t curr_pid;
    check_alive_jobs(jobs);
    for (int i = 0; i < 4; i+=1) {
        curr_job = jobs[i];
        if (curr_job.is_active == true)
        {
            curr_pid = curr_job.pid;
            kill_status = kill(curr_pid, SIGKILL);
            if (kill_status == -1)
            {
                printf("hw1shell: %s failed, errno is %d\n", "kill", errno);
                return;
            }
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
    for (i = 0; i <= len; i++) {
        if (user_input[i] == '&')
        {
            amp_ix = i;
            user_input[i] = 0;
            strcpy(instruction->raw_instruction, user_input);
            instruction->operation = INST;
            instruction->is_backround = true;
            
        }
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
    //check_alive_jobs(jobs);
    for (int i = 0; i < 4; i+=1) {
        if (jobs[i].is_active)
        {
            printf("%ld\t%s\n", jobs[i].pid, jobs[i].raw_instruction);
        }
    }
}

void cd(char* directory)
{
    int is_not_successful;
    is_not_successful = chdir(directory);
    if (is_not_successful != 0)
        printf("hw1shell: invalid command\n");
}

JobNode* get_available_job(JobNode* jobs)
{
    JobNode* curr_job;
    for (int i = 0; i < 4; i+=1) {
        curr_job = &(jobs[i]);
        if (curr_job->is_active == false)
        {
            return curr_job;
        }
    }
    return NULL;
}

char** split_instruction(char* raw_instruction)
{
    char** args;


}

void foreground_external_command(Instruction* curr_inst)
{
    pid_t child_pid;
    int exec_status;
    char path[MAX_LINE_LENGHT];
    getcwd(path, MAX_LINE_LENGHT);

    child_pid = fork();
    if (child_pid < 0)
    {
        printf("hw1shell: %s failed, errno is %d\n", "exec", errno);
        return;
    }

    if (child_pid == 0)  // we are in child
    {
        char *args[MAX_LINE_LENGHT] = {0};
        int i = 0;
        args[i] = strtok(curr_inst->raw_instruction, " ");
        while (args[i] != NULL)
        {
            args[++i] = strtok(NULL, " ");
        }
        exec_status = execvp(args[0], args);

        if (exec_status == -1)
        {
            if (errno == 2)
            {
                printf("hw1shell: invalid command\n");
                exit(1);
            }
            else
            {
                printf("hw1shell: %s failed, errno is %d\n", "execvp", errno);
                exit(1);
            }
        }
    }
    else
    {
        int child_status;
        if (waitpid(child_pid, &child_status, 0) == -1) {
            printf("hw1shell: %s failed, errno is %d\n", "waitpid", errno);
            return;
        }
    }


}

void background_external_command(Instruction* curr_inst, JobNode jobs[4])
{
    JobNode* available_job = get_available_job(jobs);
    pid_t child_pid;
    int exec_status;
    char path[MAX_LINE_LENGHT];
    getcwd(path, MAX_LINE_LENGHT);

    if (available_job == NULL)
    {
        printf("BAD!");
        exit(1);
    }

    child_pid = fork();
    if (child_pid < 0)
    {
        printf("hw1shell: %s failed, errno is %d\n", "exec", errno);
        return;
    }

    if (child_pid == 0)  // we are in child
    {
        char *args[MAX_LINE_LENGHT] = {0};
        int i = 0;
        args[i] = strtok(curr_inst->raw_instruction, " ");
        while (args[i] != NULL)
        {
            args[++i] = strtok(NULL, " ");
        }
        exec_status = execvp(args[0], args);

        if (exec_status == -1)
        {
            if (errno == 3)  // TODO
            {
                printf("hw1shell: invalid command\n");
                exit(1);
            }
            else
            {
                printf("hw1shell: %s failed, errno is %d\n", "execvp", errno);
                exit(1);
            }
        }
    }
    else
    {
        available_job->is_active = true;
        strcpy(available_job->raw_instruction, curr_inst->raw_instruction);
        available_job->pid = child_pid;
        printf("hw1shell: pid %d started\n", child_pid);
    }

}

void external_command_execute(Instruction* curr_inst, JobNode jobs[4])
{
    if (curr_inst->is_backround == true)
    {
        background_external_command(curr_inst, jobs);
    }
    else
    {
        foreground_external_command(curr_inst);
    }
    
}

void execute_input(Instruction *instruction, JobNode jobs[4])
{
    int alive_jobs = check_alive_jobs(jobs);
    if (instruction->operation == CD)
    {
        cd(instruction->directory);
    }
    else if (instruction->operation == JOBS)
    {
        jobs_func(jobs);
    }
    else {  // exit is handeled prior to this
        if (alive_jobs == 4 && instruction->is_backround == true)
        {
            printf("hw1shell$ too many background commands running\n");
        }
        else
        {
            external_command_execute(instruction, jobs);
        }
    }
}

bool allocate_jobs(JobNode* jobs, int buf_size)
{
    jobs[0].raw_instruction = (char *)malloc(buf_size);
    jobs[1].raw_instruction = (char *)malloc(buf_size);
    jobs[2].raw_instruction = (char *)malloc(buf_size);
    jobs[3].raw_instruction = (char *)malloc(buf_size);
    jobs[0].is_active = false;
    jobs[1].is_active = false;
    jobs[2].is_active = false;
    jobs[3].is_active = false;
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
    fflush(stdin);
    Instruction curr_instruction;
    size_t buf_size = MAX_LINE_LENGHT;
    char *input_buffer;
    int job_allocation_status;  // TODO - RAZ allocate space when running a new job for raw_instruction
    JobNode jobs[4];
    int alive_jobs;
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
        check_alive_jobs(jobs);
        printf("hw1shell$ ");
        fflush(stdin);
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
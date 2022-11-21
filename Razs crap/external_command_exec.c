#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>
#include "external_command_exec.h"
#define MAX_LINE_LENGHT 100
#define MAX_RUNNING_JOBS 4


//running external commands!
int ExtComExec(struct Instruction* curr_inst, struct JobNode* job_list)
{
    fflush(stdin);
    struct JobNode* job_pointer;
    char to_exec[MAX_LINE_LENGHT];
    strcpy(to_exec,curr_inst->operation);
    char path[MAX_LINE_LENGHT];
    getcwd(path, MAX_LINE_LENGHT);
    fflush(stdin);
    pid_t curr_pid;
    pid_t dead_child;
    if(curr_inst->backround)
    {
        int jobs_counter=0;
        job_pointer=job_list; 
        if(job_pointer->pid!=0)
                while(job_pointer->next!=NULL)
                {
                    job_pointer=job_pointer->next;
                    jobs_counter++;
                }
        if (jobs_counter>=4)
        {
            printf("hw1shell: too many background commands running\n");
            return 0;
        }
    }
    char *args[]= {&to_exec,"-lh", path, NULL};
    curr_pid=fork();

    //error forking
    if (curr_pid<0)
    {
        printf("error creating child process");
        return 1;
    }
    
    //father process
    if (curr_pid!=0)
    {  
        if (!curr_inst->backround)
        {
            //printf("Father -  I'm waiting for child pid %d to finish\n", curr_pid);
            int returnStatus;
            dead_child=waitpid(curr_pid, &returnStatus, 0);
            //printf("Father - child pid - %d finished!\n", dead_child);
        }
        else
        {
            //printf("Father - no need to wait, we continue\n");
            job_pointer=job_list;                
            struct JobNode* new_job=(struct JobNode*)malloc(sizeof(JobNode));
            new_job->pid=curr_pid;
            new_job->raw_instruction=to_exec;
            new_job->next=NULL;
            if(job_pointer->pid!=0)
            {
                while(job_pointer->next!=NULL)
                    job_pointer=job_pointer->next;
                job_pointer->next=new_job;
            }
            else
                job_list=new_job;
        }
    }

    //child process
    else
    {
        printf("hw1shell: pid %d started\n", curr_pid);
        execv(to_exec, args);
        //printf("Child - I finished! now I'll die\n"); //shouldn't be seen
    }
    return 0;
}
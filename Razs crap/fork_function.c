#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#ifndef <string.h>
#include <string.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>
#define MAX_LINE_LENGHT 100
#define MAX_RUNNING_JOBS 4

void PrintList(struct JobNode* job_pointer)
{
    int i=1;
    printf("Job no.%d, pid - %d, instruction - %s\n",i,job_pointer->pid,job_pointer->raw_instruction);
    while (job_pointer->next!=NULL)
    {
        i++;
        job_pointer=job_pointer->next;
        printf("Job no.%d, pid - %d, instruction - %s\n",i,job_pointer->pid,job_pointer->raw_instruction);
    }
}

int main()
{
    fflush(stdin);
    struct JobNode* job_list=(struct JobNode*)malloc(sizeof(JobNode));
    job_list->pid=0;
    job_list->next=NULL;
    struct JobNode* job_pointer;
    int background_flag=1;
    char to_exec[MAX_LINE_LENGHT];
    char path[MAX_LINE_LENGHT];
    getcwd(path, MAX_LINE_LENGHT);
    while(strcmp(&to_exec,"exit")!=0)
    {
        fflush(stdin);
        printf("enter command name to execute: ");
        scanf("%s", to_exec);
        printf("should I stay(1) or should I go(0)? ");
        scanf("%d",&background_flag);
        pid_t curr_pid;
        pid_t dead_child;
        if(background_flag==0)
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
                printf("only %d of jobs allowed", MAX_RUNNING_JOBS);
                return 0;
            }
        }
        char *args[]= {&to_exec,"-lh", path, NULL};
        curr_pid=fork();

        //error forking
        if (curr_pid<0)
        {
            printf("error creating child process");
        }
        
        //father process
        if (curr_pid!=0)
        {  
            if (background_flag==1)
            {
                printf("Father -  I'm waiting for child pid %d to finish\n", curr_pid);
                int returnStatus;
                dead_child=waitpid(curr_pid, &returnStatus, 0);
                printf("Father - child pid - %d finished!\n", dead_child);
            }
            else
            {
                printf("Father - no need to wait, we continue\n");
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
                PrintList(job_list);
            }
        }

        //child process
        else
        {
            printf("I'm the child, my pid is - %d and im execing -  %s\n", curr_pid, to_exec);
            execv(to_exec, args);
            printf("Child - I finished! now I'll die\n"); //shouldn't be seen
        }
    }
    return 0;
}
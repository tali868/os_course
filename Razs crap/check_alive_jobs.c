#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>
#include "check_alive_jobs.h"
#define MAX_LINE_LENGHT 100
#define MAX_RUNNING_JOBS 4


//recieves a pointer to the Jobs List header and discard all dead processes from it
void CheckAliveJobs (struct JobNode* jobs_pointer)
{
    int status;
    int curr_status;
    struct JobNode* curr_job=jobs_pointer;
    struct JobNode* discard_job;
    if (curr_status=waitpid(curr_job->pid,&status,WNOHANG)==0)
    {
        discard_job=curr_job;
        jobs_pointer=jobs_pointer->next;
        curr_job=jobs_pointer;
        free(discard_job); 
    }
    while(curr_job->next!=NULL)
    {
        curr_job=curr_job->next;
        if(curr_status=waitpid(curr_job->pid,&status,WNOHANG)==0)
        {
            discard_job=curr_job;
            curr_job=curr_job->next;
            free(discard_job);
        }    
    }
    
}
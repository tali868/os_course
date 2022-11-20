#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>
#define MAX_LINE_LENGHT 100
#define MAX_RUNNING_JOBS 4


//recieves a pointer to the Jobs List header and discard all dead processes from it
void CheckAliveJobs (struct JobNode* jobs_pointer)
{
    struct JobNode* curr_job=jobs_pointer;
    struct JobNode* discard_job;
    if (kill(curr_job->pid,0)!=0)
    {
        discard_job=curr_job;
        jobs_pointer=jobs_pointer->next;
        curr_job=jobs_pointer;
        free(discard_job); 
    }
    while(curr_job->next!=NULL)
    {
        curr_job=curr_job->next;
        if(kill(curr_job->pid,0)!=0)
        {
            discard_job=curr_job;
            curr_job=curr_job->next;
            free(discard_job);
        }    
    }
    
}
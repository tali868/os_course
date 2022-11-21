#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>

#define MAX_LINE_LENGHT 100
#define MAX_RUNNING_JOBS 4

void CheckAliveJobs (struct JobNode* jobs_pointer);
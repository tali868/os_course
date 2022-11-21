#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>
#define MAX_LINE_LENGHT 100
#define MAX_RUNNING_JOBS 4

int ExtComExec(struct Instruction* curr_inst, struct JobNode* job_list);
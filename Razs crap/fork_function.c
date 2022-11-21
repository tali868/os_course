#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include </home/ubuntu/Repos/HW_OS/ass_1/shell.h>
#define MAX_LINE_LENGHT 100

int main()
{
    int background_flag=1;
    char to_exec[MAX_LINE_LENGHT];
    char path[MAX_LINE_LENGHT];
    getcwd(path, MAX_LINE_LENGHT);
    printf("enter command name to execute: ");
    scanf("%s", to_exec);
    printf("should I stay(1) or should I go(0)? ");
    scanf("%d",&background_flag);
    char *args[]= {&to_exec,"-lh", path, NULL};
    pid_t curr_pid=fork();
    pid_t dead_child;

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
        }
        return 0;
    }

    //child process
    else
    {
        printf("I'm the child, my pid is - %d and im execing -  %s\n", curr_pid, to_exec);
        execv(to_exec, args);
        printf("Child - I finished! now I'll die\n"); //shouldn't be seen
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

pid_t child_pid=-1;

void kill_child(int sig)
{
    kill(child_pid,SIGKILL);
}

int main()
{
    int i=0;
    char user_input[5];
    char s[100];
    for (i=0;i<=10;i++)
    {
        //just to have breakpoint
        scanf("%s", user_input);

        // printing current working directory
        printf("%s\n", getcwd(s, 100));
  
        // using the command
        chdir("..");
  
        // printing current working directory
        printf("%s\n", getcwd(s, 100));

        //create child process
        signal(SIGALRM,(void (*)(int))kill_child);
        child_pid = fork();
        if (child_pid > 0) {
        /*PARENT*/
            alarm(30);
            /*
            * Do parent's tasks here.
            */
            wait(NULL);
        }
        else if (child_pid == 0){
        /*CHILD*/
            /*
            * Do child's tasks here.
            */
    }

        //print num of loop
        printf("%s, %d\n",user_input, i);
    }
    return 0;
}
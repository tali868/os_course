#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define MAX_LINE_LENGHT 100


int main()
{
    //need to get the wanted directory from the pharser!!
    char user_input[MAX_LINE_LENGHT]="a"; //the wanted directory!

    while (strcmp(user_input,"quit")!=0)
    {
        printf("current directory: %s\n", getcwd(user_input, MAX_LINE_LENGHT));
        printf("enter the wanted directory: ");
        scanf("%s",user_input);
        int flag=0;

        flag=chdir(user_input);
        if (flag != 0) 
            // so chdir will return -1 
            printf("chdir() to %s failed, try again\n", user_input);

        //printf("new current directory: %s\n", getcwd(user_input, MAX_LINE_LENGHT));
    }
    return 0;
}
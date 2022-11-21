#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define MAX_LINE_LENGHT 100

int main()
{
    printf("Waiting - running 15 sec timer\n");
    sleep(15);
    printf("Waiting - finished\n");
    sleep(1);
    return 0;
}
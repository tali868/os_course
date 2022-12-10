#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "exec_functions.h"

void* read_and_execute(void *input) {
    Queue *q = ((struct args*)input)->q;
    pthread_mutex_t queue_lock = ((struct args*)input)->queue_lock;
    pthread_mutex_t* files_lock = ((struct args*)input)->files_lock;
    FILE** count_files = ((struct args*)input)->count_files;

    while (1) {
        char* command;
        pthread_mutex_lock(&queue_lock);

        // Check if queue is empty
        if (q->head == NULL) {
            pthread_mutex_unlock(&queue_lock);
            continue;
        }
        duplicate_on_repeat(q->head);
        char* commands = q->head->data;
        dequeue(q);  // Pop the command from the queue
        pthread_mutex_unlock(&queue_lock);

        command = strtok(commands, "; ");
        while (command != NULL)
        {
            run_command_line(command, files_lock, count_files);
            command = strtok(NULL, "; ");
        }
        // Free the memory of the command
        free(command);  // TODO - does this needs to be done?
    }
}

void run_command_line(char* command_line, pthread_mutex_t* files_lock, FILE** count_files)
{
    int i = 0;
    int len = strlen(command_line);
    int num = (int) (command_line + len - 2);
    if (strstr(command_line, "msleep") != NULL) {
        sleep(num);
    }
    else{
        fscanf(*(count_files + num), "%d", &i);
        if (strstr(command_line, "increment") != NULL) {
            i++;
        }
        if (strstr(command_line, "decrement") != NULL) {
            i--;
        }
        fprintf(*(count_files + num),"%d", i);
        fflush(*(count_files + num));
    sleep(3);
    }
}

void execute_dispatcher(char *command, pthread_t* threads, Queue *q) {
    if (strcmp(command, "wait") == 0) {
        while (q->head != NULL)  // TODO: not sure that this is what has to be done!!!
        {
            sleep(1);
        }
    }
    else  // sleep is left as an option
    {
        int len = strlen(command);
        int num = atoi(command + len - 1);
        sleep(num);
    }
}

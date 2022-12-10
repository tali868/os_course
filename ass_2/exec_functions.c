#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "exec_functions.h"

void wait(int num_threads, int* is_busy)
{
    int should_wait = 1, running_threads=0;
    while (should_wait == 1)  // while we are not done, we can't end the program
    {
        for (int i = 0; i < 100; i++) {
            if(*(is_busy + i) == 1)
            {
                running_threads++;
            }
        }
        if (running_threads == 0)  // All threads are finished now and the program can end
        {
            should_wait = 0;
        }
        else
        {
            running_threads = 0;
        }
    }
}

void* read_and_execute(void *input) {
    Queue *q = ((struct args*)input)->q;
    pthread_mutex_t queue_lock = ((struct args*)input)->queue_lock;
    pthread_mutex_t files_lock = ((struct args*)input)->files_lock;
    char** count_files = ((struct args*)input)->count_files;
    int* is_busy = ((struct args*)input)->is_busy;

    while (1) {
        char* command = (char*) malloc(12*sizeof(char));
        char* commands = (char*) malloc(6*MAX_LINE_LENGTH*sizeof(char));
        int* indexes[MAX_LINE_LENGTH];
        int k=0;
        int start_ix, end_ix;
        pthread_mutex_lock(&queue_lock);
        *is_busy = 1;

        // Check if queue is empty
        if (q->head == NULL) {
            pthread_mutex_unlock(&queue_lock);
            continue;
        }
        duplicate_on_repeat(q->head);
        memcpy(commands, q->head->data, strlen(q->head->data));
        dequeue(q);
        pthread_mutex_unlock(&queue_lock);

        for (int i = 0; i < strlen(commands); i++) { 
  
            // Check if the current character is ";" 
            if (commands[i] == ';') { 
  
                // Store the index of the current character 
                indexes[k] = i; 
                k++; 
            } 
        }
        if (k == 0)
        {
            run_command_line(commands, files_lock, count_files);
        }
        else
        {
            for (int i = 0; i < k; i++)
            {
                if (i == 0)
                {
                    start_ix = 0;
                }
                else
                {
                    start_ix = (int) *(indexes + i - 1) + 2;
                }
                end_ix = (int) *(indexes + i);
                memcpy(command, commands + start_ix, end_ix - start_ix);
                *(command + end_ix - start_ix) = 0;
                run_command_line(command, files_lock, count_files);
            }
            start_ix = (int) *(indexes + k - 1) + 2;
            end_ix = strlen(commands);
            memcpy(command, commands + start_ix, end_ix - start_ix);
            *(command + end_ix - start_ix) = 0;
            run_command_line(command, files_lock, count_files);
        }
        free(commands);
        free(command);
        *is_busy = 0;
    }
}

void run_command_line(char* command_line, pthread_mutex_t files_lock, char** count_files)
{
    int i = -1;
    int len = strlen(command_line);
    int num;
    char* str_num = (char*) malloc(6*sizeof(char));
    FILE* temp_count_file;
    
    memcpy(str_num, command_line + 7, len - 7);
    *(str_num + len - 7) = 0;
    num = atoi(str_num);
    if (strstr(command_line, "msleep") != NULL) {
        memcpy(str_num, command_line + 7, len - 7);
        *(str_num + len - 7) = 0;
        num = atoi(str_num);
        sleep(num);
    }
    else{
        memcpy(str_num, command_line + 9, len - 9);
        *(str_num + len - 9) = 0;
        num = atoi(str_num);
        pthread_mutex_lock(&files_lock);
        temp_count_file = fopen(*(count_files + num), "r");
        fscanf(temp_count_file, "%d", &i);
        fclose(temp_count_file);
        if (strstr(command_line, "increment") != NULL) {
            i++;
        }
        if (strstr(command_line, "decrement") != NULL) {
            i--;
        }
        temp_count_file = fopen(*(count_files + num), "w");
        fprintf(temp_count_file,"%d", i);
        fclose(temp_count_file);
        pthread_mutex_unlock(&files_lock);
    }
}

void execute_dispatcher(char *command, pthread_t* threads, Queue *q, int* is_busy) {
    char* str_num = (char*) malloc(6*sizeof(char));
    if (strcmp(command, "wait") == 0) {
        wait(MAX_NUM_THREADS, is_busy);
    }
    else  // sleep is left as an option
    {
        int len = strlen(command);
        memcpy(str_num, command + 7, len - 7);
        *(str_num + len - 7) = 0;
        int num = atoi(str_num);
        sleep(num);
    }
}

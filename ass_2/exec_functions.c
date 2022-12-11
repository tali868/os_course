#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "exec_functions.h"

pthread_mutex_t queue_mutex;
pthread_mutex_t files_mutex;

long long timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (((long long)t1.tv_sec - (long long)t0.tv_sec)*1000)+((t1.tv_usec - t0.tv_usec)/1000);
}

void wait(int num_threads, int** is_busy, Queue *q)
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
        if (running_threads == 0 && q->head == NULL)  // All threads are finished now and the program can end
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
    char** count_files = ((struct args*)input)->count_files;
    int* is_busy = ((struct args*)input)->is_busy;
    struct timeval stop, start = ((struct args*)input)->time_of_run;
    long long int* total_runtime = ((struct args*)input)->total_runtime;
    int is_log_enabled = ((struct args*)input)->is_log_enabled;

    FILE* thread_log_file;
    char thread_log_file_name[13];
    snprintf(thread_log_file_name, 13, "thread%d.txt", ((struct args*)input)->thread_id);

    while (1) {
        int* indexes[MAX_LINE_LENGTH];
        int k=0;
        int start_ix, end_ix;
        *is_busy = 1;
        gettimeofday(&stop, NULL);

        pthread_mutex_lock(&queue_mutex);
        if (q->head == NULL) {  // Check if queue is empty
            pthread_mutex_unlock(&queue_mutex);
            continue;
        }
        char* command = (char*) malloc(12*sizeof(char));
        char* commands = (char*) malloc(6*MAX_LINE_LENGTH*sizeof(char));
        char* orig_commands = (char*) malloc(6*MAX_LINE_LENGTH*sizeof(char));
        memcpy(orig_commands, q->head->data, strlen(q->head->data));
        duplicate_on_repeat(q->head);
        memcpy(commands, q->head->data, strlen(q->head->data));
        dequeue(q);
        pthread_mutex_unlock(&queue_mutex);

        if (is_log_enabled)
        {
            thread_log_file = fopen(thread_log_file_name, "a");
            fprintf(thread_log_file, "TIME %lld: START job %s\n", timedifference_msec(start, stop), orig_commands);
            fclose(thread_log_file);
        }
        
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
            run_command_line(commands, count_files);
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
                run_command_line(command, count_files);
            }
            start_ix = (int) *(indexes + k - 1) + 2;
            end_ix = strlen(commands);
            memcpy(command, commands + start_ix, end_ix - start_ix);
            *(command + end_ix - start_ix) = 0;
            run_command_line(command, count_files);
        }
        
        gettimeofday(&stop, NULL);
        ((struct args*)input)->total_runtime = timedifference_msec(start, stop);  // save runtime for statistics
        if (is_log_enabled == 1)
        {    
            thread_log_file = fopen(thread_log_file_name, "a");
            fprintf(thread_log_file, "TIME %lld: END job %s\n", ((struct args*)input)->total_runtime, orig_commands);
            fclose(thread_log_file);
        }
        
        free(commands);
        free(command);
        free(orig_commands);
        *is_busy = 0;
        sleep(1);
    }
}

void run_command_line(char* command_line, char** count_files)
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
        pthread_mutex_lock(&files_mutex);
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
        pthread_mutex_unlock(&files_mutex);
    }
}

void execute_dispatcher(char *command, pthread_t* threads, Queue *q, int** is_busy) {
    char* str_num = (char*) malloc(6*sizeof(char));
    if (strcmp(command, "wait") == 0) {
        wait(MAX_NUM_THREADS, is_busy, q);
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

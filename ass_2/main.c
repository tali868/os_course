#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include "consts.h"
#include "structs.h"
#include "queue.h"
#include "parsing.h"
#include "exec_functions.h"


void duplicate_on_repeat(QueueNode* q)
{
    char* result;
    char* command_line = q->data;
    int repeat_num, j, after_increase_ix, new_ix;
    int line_len = strlen(command_line);
    for(int i = 0; i < line_len; i++){
        if(command_line[i] == 'r' && command_line[i+1] == 'e' && 
            command_line[i+2] == 'p' && command_line[i+3] == 'e' && 
            command_line[i+4] == 'a' && command_line[i+5] == 't') {
            // if found, store the number
            repeat_num = (int) (command_line[i+7]) - 48;
            // allocate memory for the result string
            result = (char*) malloc((i + (line_len - 8) * repeat_num + 1)*sizeof(char));
            // copy 1st part
            for(j = 0; j < i; j++){
                result[j] = command_line[j];
            }
            new_ix = j;
            after_increase_ix = j + 9;
            // copy 2nd part x times
            for(int k = 0; k < repeat_num; k++) {
                for(j = after_increase_ix + 1; j < line_len; j++){
                    result[new_ix] = command_line[j];
                    new_ix++;
                }
                if (k != repeat_num - 1)
                {
                    result[new_ix++] = ';';
                    result[new_ix++] = ' ';
                }
            }
            free(q->data);
            q->data = result;
            break;
        }
    }
}


int main(int argc, char *argv[])
{
    int i;
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    int* is_busy[MAX_THREAD_COUNT] = {0};
    char file_num_name[13];
    char buffer[MAX_LINE_LENGTH];
    size_t buffer_len = MAX_LINE_LENGTH;
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int blah = -1;
    
    FILE* commands_file;
    FILE* temp_count_file;
    char** count_files = (char*) malloc(sizeof(char*) * MAX_NUM_FILES);
    Queue *queue = create_queue();
    
    pthread_t threads[MAX_NUM_THREADS];
    pthread_mutex_t queue_mutex;
    pthread_mutex_t files_mutex;

    struct args *thread_input = (struct args *)malloc(sizeof(struct args));

    for (int i=0; i<num_counters; i++)
    {
        count_files[i] = (char*) malloc(sizeof(char*) * 13);
        snprintf(file_num_name, 13, COUNT_FILE_NAME_TEMPLATE, i);
        temp_count_file = fopen(file_num_name, "w");
        if (temp_count_file == NULL)
	    {
            printf("%s failed, errno is %d\n", "fopen", errno);
		    printf("Counter file isn't created. Please check and run again.");
		    exit(1);
	    }
        fprintf(temp_count_file, "0");
        fflush(temp_count_file);
        fclose(temp_count_file);
        strcpy(count_files[i], file_num_name);
    }

    commands_file = fopen(argv[1], "r");
	
	// open input files
	if (commands_file == NULL)
	{
		printf("Commands file doesn't open. Please check and run again.");
		exit(1);
	}
    thread_input->count_files = count_files;
    thread_input->files_lock = files_mutex;
    thread_input->queue_lock = queue_mutex;
    thread_input->q = queue;
    thread_input->time_of_run = start;

    // create the threads with the function to wait for queue
    for (int i = 0; i < num_threads; i++) {
        thread_input->is_busy = (is_busy + i);
        thread_input->thread_id = i;
        pthread_create(&threads[i], NULL, read_and_execute, (void*)thread_input);
    }

    while ((fgets(buffer, buffer_len, commands_file)) != NULL) {
        if (buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = 0;
        }
        if (is_worker(buffer) == true)
        {
            push_worker_to_queue(queue, buffer);
        }
        else
        {
            execute_dispatcher(buffer + 11, threads, queue, is_busy);
        }
    }
    wait(num_threads, is_busy);

    free(count_files);
    free_queue(queue);

    exit(0);
}
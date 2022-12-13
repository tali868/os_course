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
            result[new_ix] = 0;
            q->data = result;
            break;
        }
    }
}


void get_running_times(long long int *total_running_times, struct args **all_args, int num_threads)
{
    for (int i = 0; i < num_threads; i++) 
    {
        *(total_running_times + i) = (all_args[i])->total_runtime;
    }
}


long long sum_running_times(long long int *total_running_times, int num_threads)
{
    long long int sum = 0;
    for (int i = 0; i < num_threads; i++) 
    {
        sum = sum + *(total_running_times + i);
    }
    return sum;
}

long long max_running_times(long long int *total_running_times, int num_threads)
{
    long long int max = 0, temp;
    for (int i = 0; i < num_threads; i++) 
    {
        if (i == 0)
        {
            max = *(total_running_times);
        }
        else
        {
            temp = *(total_running_times + i);
            if (temp > max)
            {
                max = temp;
            }
        }
    }
    return max;
}

long long min_running_times(long long int *total_running_times, int num_threads)
{
    long long int min = 0, temp;
    for (int i = 0; i < num_threads; i++) 
    {
        if (i == 0)
        {
            min = *(total_running_times);
        }
        else
        {
            temp = *(total_running_times + i);
            if (temp < min)
            {
                min = temp;
            }
        }
    }
    return min;
}

float avg_running_times(long long int *total_running_times, int num_threads)
{
    long long int sum = sum_running_times(total_running_times, num_threads);
    float num_threads_f = (float) num_threads;
    return (float)sum/num_threads_f;
}


int main(int argc, char *argv[])
{
    int i;
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    int is_log_enabled = atoi(argv[4]);
    int* is_busy[MAX_THREAD_COUNT] = {0};
    char file_num_name[13];
    char buffer[MAX_LINE_LENGTH];
    size_t buffer_len = MAX_LINE_LENGTH;
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int blah = -1;
    
    FILE* commands_file;
    FILE* temp_count_file;
    FILE* dispatcher_log_file;
    FILE* stats_file;
    char** count_files = (char*) malloc(sizeof(char*) * MAX_NUM_FILES);
    Queue *queue = create_queue();
    
    pthread_t threads[MAX_NUM_THREADS];

    // struct args *thread_input = (struct args *)malloc(sizeof(struct args));
    struct args *all_args[MAX_NUM_THREADS];
    long long int total_running_times[MAX_NUM_THREADS];

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
    

    // create the threads with the function to wait for queue
    for (int i = 0; i < num_threads; i++) {
        struct args *thread_input = (struct args *)malloc(sizeof(struct args));
        thread_input->count_files = count_files;
        thread_input->q = queue;
        thread_input->is_busy = (is_busy + i);
        thread_input->thread_id = i;
        thread_input->time_of_run = start;
        thread_input->is_log_enabled = is_log_enabled;
        all_args[i] = thread_input;
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
	    if (is_log_enabled)
	    {
            	gettimeofday(&stop, NULL);
            	dispatcher_log_file = fopen("dispatcher.txt", "w");
            	fprintf(dispatcher_log_file, "TIME %lld: START job %s\n", timedifference_msec(start, stop), buffer);
            }
	    execute_dispatcher(buffer + 11, threads, queue, is_busy);
            if (is_log_enabled)
	    {
	    	gettimeofday(&stop, NULL);
            	fprintf(dispatcher_log_file, "TIME %lld: END job %s\n", timedifference_msec(start, stop), buffer);
            	fclose(dispatcher_log_file);
	    }
        }
    }
    wait(num_threads, is_busy, queue);
    get_running_times(total_running_times, all_args, num_threads);

    gettimeofday(&stop, NULL);
    stats_file = fopen("stats.txt", "w");
    fprintf(stats_file, "total running time: %lld milliseconds\n", timedifference_msec(start, stop));
    fprintf(stats_file, "sum of jobs turnaround time: %lld milliseconds\n", sum_running_times(total_running_times, num_threads));
    fprintf(stats_file, "min job turnaround time: %lld milliseconds\n", min_running_times(total_running_times, num_threads));
    fprintf(stats_file, "average job turnaround time: %f milliseconds\n", avg_running_times(total_running_times, num_threads));
    fprintf(stats_file, "max job turnaround time: %lld milliseconds\n", max_running_times(total_running_times, num_threads));
    fclose(stats_file);
    
    
    
    for (int i = 0; i < num_threads; i++)
    {
        free(all_args[i]);
    }
    free(count_files);
    free_queue(queue);

    exit(0);
}

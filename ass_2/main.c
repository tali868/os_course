#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "consts.h"
#include "structs.h"

Queue* create_queue(){
	Queue *q = (Queue*)malloc(sizeof(Queue));
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	return q;
}

// Function to add an item to the queue
void enqueue(Queue *q, char *data){
	// Create a new node
	QueueNode *temp = (QueueNode*) malloc(sizeof(QueueNode));
    QueueNode *p;
    temp->data = data;
	temp->next = NULL;

	p = q->head;
    if (p == NULL)
    {
        q->head = temp;
        return;
    }
    while (p->next)
        p = p->next;
    p->next = temp;
}


void dequeue(Queue *q)
{
    if (q->head == NULL)
    {
        printf("Queue is Empty");  // TODO
    }
    QueueNode *temp = q->head;
    q->head = q->head->next;
    if (q->head == NULL)  // TODO
       q->tail = NULL;
    free(temp);
}


bool is_worker(char *line){
	char *worker = "worker";
	// Check if the first 5 characters of the line are "worker"
	if(strncmp(line, worker, 5) == 0){
		return true;
	}
	return false;
}

bool is_dispatcher(char *line){
	char *dispatcher = "dispatcher";
	// Check if the first 9 characters of the line are "dispatcher"
	if(strncmp(line, dispatcher, 9) == 0){
		return true;
	}
	return false;
}


// Function to push the rest of the line to a queue
void push_worker_to_queue(Queue *q, char *line){
	int i;
	int len = strlen(line);
	char *data = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));

	// Copy the rest of the line to the data
	for(i = 7; i < len; i++){
		data[i - 7] = line[i];
	}
	data[i-7] = '\0';  // TODO - bugggg?

	// Push the data to the queue
	enqueue(q, data);
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
        int num = (int) (command + len - 2);
        sleep(num);
    }
}

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

void* read_and_execute(void *input) {
    Queue *q = ((struct args*)input)->q;
    pthread_mutex_t queue_lock = ((struct args*)input)->queue_lock;
    pthread_mutex_t* files_lock = ((struct args*)input)->files_lock;
    FILE** count_files = ((struct args*)input)->count_files;

    while (true) {
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

//Function to free queue
void free_queue(Queue *q)
{
    QueueNode *temp;
    while (q->head != NULL)
    {
        temp = q->head;
        q->head = q->head->next;
        free(temp);
    }
    q->tail = NULL;
    free(q);
}


int main(int argc, char *argv[])
{
    int i;
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    char file_num_name[13];
    char line[MAX_LINE_LENGTH];
    size_t len = MAX_LINE_LENGTH;
    
    FILE* commands_file;
    FILE* count_files[MAX_FILE_COUNTER];
    Queue *queue = create_queue();
    
    pthread_t threads[MAX_NUM_THREADS];
    pthread_mutex_t queue_mutex;
    pthread_mutex_t files_mutex[MAX_FILE_COUNTER];

    struct args *thread_input = (struct args *)malloc(sizeof(struct args));

    for (int i=0; i<num_counters; i++)
    {
        snprintf(file_num_name, 13, TREAD_FILE_NAME_TEMPLATE, i);
        count_files[i] = fopen(file_num_name, "w");
        if (count_files[i] == NULL)
	    {
            printf("%s failed, errno is %d\n", "waitpid", errno);
		    printf("Counter file isn't created. Please check and run again.");
		    exit(1);
	    }
        fprintf(count_files[i], "0");
        fflush(count_files[i]);
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

    // ---------------- //
    // create the threads with the function to wait for queue
    // for (int i = 0; i < num_threads; i++) {
    //     pthread_create(&threads[i], NULL, read_and_execute, (void*)thread_input);
    // }
    // ---------------- //
    // char demi_line[48] = "worker msleep 1; repeat 2; increment 5; msleep 2";
    // int is_work = is_worker(demi_line);
    // if (is_work)
    // {
    //     push_worker_to_queue(queue, demi_line);
    //     duplicate_on_repeat(queue->head);
    //     char* commands = queue->head->data;
    //     dequeue(queue);  // Pop the command from the queue
    // }
    // ---------------- //

    while ((getline(&line, &len, commands_file)) != -1) {
        if (is_worker(line) == true)
        {
            push_worker_to_queue(queue, line);
        }
        else
        {
            execute_dispatcher(line + 10, threads, queue);
        }
    }


    

    // close all files
    fclose(commands_file);
    for (int i=0; i<num_counters; i++)
    {
        fclose(count_files[i]);
    }

    free_queue(queue);

    exit(0);
}
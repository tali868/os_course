#ifndef STRUCTS_H 
#define STRUCTS_H

#include <pthread.h>
#include <sys/types.h>

#define MAX_LINE_LENGTH 1024
#define MAX_NUM_THREADS 4096
#define MAX_NUM_FILES 100

typedef struct QueueNode{
	char *data;
	struct QueueNode *next;
} QueueNode;

typedef struct Queue{
	QueueNode *head;
	QueueNode *tail;
	int size;
} Queue;

struct args {
    Queue *q;
    char** count_files;
    int* is_busy;
    struct timeval time_of_run;
    long long int* total_runtime;
    int thread_id;
    int is_log_enabled;
};

#endif
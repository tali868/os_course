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
    pthread_mutex_t queue_lock;
    pthread_mutex_t files_lock;
    char** count_files;
    int* is_busy;
};

#endif
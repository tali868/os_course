#ifndef STRUCTS_H 
#define STRUCTS_H

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
    pthread_mutex_t* files_lock;
    FILE** count_files;
};

#endif
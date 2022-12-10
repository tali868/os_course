#ifndef QUEUE_H 
#define QUEUE_H

#include <pthread.h>
#include "structs.h"

Queue* create_queue();
void enqueue(Queue *q, char *data);
void dequeue(Queue *q);
void free_queue(Queue *q);
void push_worker_to_queue(Queue *q, char *line);

#endif
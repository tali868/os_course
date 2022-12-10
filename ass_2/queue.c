#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "queue.h"

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
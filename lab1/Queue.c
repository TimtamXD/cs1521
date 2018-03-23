// ADT for a FIFO queue
// COMP1521 17s2 Week01 Lab Exercise
// Written by John Shepherd, July 2017
// Modified by ...

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Queue.h"

typedef struct QueueNode {
   int jobid;  // unique job ID
   int size;   // size/duration of job
   struct QueueNode *next;
} QueueNode;

struct QueueRep {
   int nitems;      // # of nodes
   QueueNode *head; // first node
   QueueNode *tail; // last node
};

// create a new node for a Queue
static QueueNode *makeQueueNode(int id, int size)
{
   QueueNode *new;
   new = malloc(sizeof(struct QueueNode));
   assert(new != NULL);
   new->jobid = id;
   new->size = size;
   new->next = NULL;
   return new;
}


// make a new empty Queue
Queue makeQueue()
{
   Queue new;
   new = malloc(sizeof(struct QueueRep));
   assert(new != NULL);
   new->nitems = 0; new->head = new->tail = NULL;
   return new;
}

// release space used by Queue
void  freeQueue(Queue q)
{
   assert(q != NULL);
   // TODO
    struct QueueNode *a, *b;
    a = q->head;
    if (a != NULL) {
        b = q->head->next;
        while (b != NULL) {
            free(a);
            a = b;
            b = a->next;
        }
        free(a);
    }

}

// add a new item to tail of Queue
void  enterQueue(Queue q, int id, int size)
{
   assert(q != NULL);
   // TODO
   QueueNode *new = makeQueueNode(id, size);
    if (q->head == NULL) {
        q->head = q->tail = new;
        q->nitems++;
    } else {
        q->tail->next = new;
        q->tail = new;
        q->nitems++;
    }
}

// remove item on head of Queue
int   leaveQueue(Queue q)
{
   assert(q != NULL);
   // TODO
    int x = 0;
    if (q->head != NULL) {
        x = q->head->jobid;
        QueueNode *new = q->head->next;
        QueueNode *old = q->head;
        q->head = new;
        free(old);
    }
    q->nitems--;
    
   return x;
}

// count # items in Queue
int   lengthQueue(Queue q)
{
   assert(q != NULL);
   return q->nitems;
}

// return total size in all Queue items
int   volumeQueue(Queue q)
{
   assert(q != NULL);
   // TODO
    int sum = 0;
    QueueNode *top = q->head;
    for (int i = 0; i < q->nitems; i++) {
        sum = top->size + sum;
        top = top->next;
    }
   return sum;
}

// return size/duration of first job in Queue
int   nextDurationQueue(Queue q)
{
   assert(q != NULL);
   // TODO
    int num = 0;
    if (q->head != NULL) {
        num = q->head->size;
    }
   return num;
}


// display jobid's in Queue
void showQueue(Queue q)
{
   QueueNode *curr;
   curr = q->head;
   while (curr != NULL) {
      printf(" (%d,%d)", curr->jobid, curr->size);
      curr = curr->next;
   }
}


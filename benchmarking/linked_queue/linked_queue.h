#ifndef PHILO_LINKED_QUEUE_H
#define PHILO_LINKED_QUEUE_H

typedef struct queue_node t_node;

struct queue_node
{
	int data, data2, data3;
	t_node *next;
};

void mtx_init();
void mtx_enqueue(int data, int data2, int data3);
int mtx_dequeue();

# endif

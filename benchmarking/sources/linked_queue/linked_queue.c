//
// Created by Lommy greenhands Reznak mo reznak on 8/2/21.
//

#include "linked_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

t_node *head = NULL;
t_node *tail = NULL;
size_t 	sz = 0;
pthread_mutex_t lock;

void mtx_init()
{
	if (pthread_mutex_init(&lock, NULL) != 0) {
		fprintf(stderr, "mutex init failed\n");
		exit (-1);
	}
}

void mtx_enqueue(int data, int data2, int data3)
{
	t_node *new_node = (t_node *)malloc(sizeof (t_node));
	int rc;
	if (!new_node) {
		fprintf(stderr, "cannot malloc\n");
		exit(-1);
	}
	memset(new_node, 0, sizeof *new_node);
	new_node->data = data;
	new_node->data2 = data2;
	new_node->data3 = data3;

	rc = pthread_mutex_lock(&lock);
	if (rc) {
		fprintf(stderr, "Cannot lock: %d\n", rc);
		exit(EXIT_FAILURE);
	}
	//////////////////////////
	if (sz == 0)
	{
		head = new_node;
		tail = head;
	}
	else
	{
		tail->next = new_node;
		tail = new_node;
	}
	sz++;
	///////////////////////////
	rc = pthread_mutex_unlock(&lock);
	if (rc) {
		fprintf(stderr, "Cannot unlock: %d\n", rc);
		exit(EXIT_FAILURE);
	}

}


int mtx_dequeue()
{
	int ret, rc;
	t_node  *nd;

	while (1) {
		rc = pthread_mutex_lock(&lock);
		if (rc) {
			fprintf(stderr, "Cannot lock: %d\n", rc);
			exit(EXIT_FAILURE);
		}
		//////////////////////////
		if (sz) {
			ret = head->data;
			nd = head;
			head = head->next;

			free(nd);
			sz--;
			pthread_mutex_unlock(&lock);
			return ret;
		}
		//////////////////////////
		rc = pthread_mutex_unlock(&lock);
		if (rc) {
			fprintf(stderr, "Cannot unlock: %d\n", rc);
			exit(EXIT_FAILURE);
		}
	}
}
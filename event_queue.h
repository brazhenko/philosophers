#ifndef QUEUE_H
# define QUEUE_H

# include "philo.h"

/*
 * event queue module
 *
 * Thread-safe lock-free queue implementation.
 * Number enqueue() threads MUST be <= EVENT_QUEUE_SIZE.
 */
# define EVENT_QUEUE_SIZE	1024

/*
 *  struct s_philo_event -- event structure
 */
struct s_philo_event
{
	t_usec ts            	__attribute__ ((aligned(8)));
	enum e_status ev_type   __attribute__ ((aligned(8)));
	size_t philo_id    		__attribute__ ((aligned(8)));
};

/*
 * enqueue()
 *
 * Thread-safe function to push_back() an event entry to internal
 * queue of size EVENT_QUEUE_SIZE (FIFO).
 */
void					enqueue(t_usec ts, enum e_status ev_type,
									size_t philo_id);

/*
 * dequeue()
 *
 * Function to push_front() an event entry from internal queue (FIFO).
 * thread.
 *
 * N.B. Thread-safety is supported for ONE thread, i.e. some threads
 * call enqueue() concurrently and one calls dequeue().
 */
struct s_philo_event	dequeue();

#endif

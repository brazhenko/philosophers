#ifndef EVENT_QUEUE_H
# define EVENT_QUEUE_H

# include "philo.h"

/*
 * event queue module
 *
 * Thread-safe lock-free queue implementation.
 * Number ev_enqueue() threads MUST be <= EVENT_QUEUE_SIZE.
 */
# define EVENT_QUEUE_SIZE	(10240)

/*
 *  struct s_philo_event -- event structure
 */
struct s_philo_event
{
	t_usec ts				ALIGN_8;
	enum e_status ev_type	ALIGN_8;
	size_t philo_id			ALIGN_8;
};

/*
 * ev_enqueue()
 *
 * Thread-safe function to push_back() an event entry to internal
 * queue of size EVENT_QUEUE_SIZE (FIFO).
 */
void					ev_enqueue(t_usec ts,
							enum e_status ev_type,
							size_t philo_id);

/*
 * ev_dequeue()
 *
 * Function to pop_front() an event entry from internal queue (FIFO).
 * thread.
 *
 * N.B. Thread-safety is supported for ONE thread, i.e. some threads
 * call ev_enqueue() concurrently and one calls ev_dequeue().
 */
struct s_philo_event	ev_dequeue(void);

/*
 * ev_clean()
 *
 * Erases all internal structures
 *
 * N.B. not thread safe
 */
void					ev_clean(void);

#endif

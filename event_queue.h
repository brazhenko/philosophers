#ifndef QUEUE_H
# define QUEUE_H

# include "philo.h"
# define QUEUE_SIZE	1024

struct s_philo_event
{
	t_usec ts            __attribute__ ((aligned(8)));
	enum e_status ev_type    __attribute__ ((aligned(8)));
	size_t philo_id    __attribute__ ((aligned(8)));
};

/*
 * enqueu
 */

void					enqueue(t_usec ts, enum e_status ev_type, size_t philo_id);
struct s_philo_event	deque();

#endif

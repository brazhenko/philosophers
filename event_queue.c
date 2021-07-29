#include "philo.h"
#include "event_queue.h"
#include "atomic_primitives.h"
#include <stdio.h>
#include <unistd.h>

struct s_philo_event_internal
{
	t_usec			ts 			__attribute__ ((aligned(8)));
	enum e_status	ev_type 	__attribute__ ((aligned(8)));
	size_t			philo_id	__attribute__ ((aligned(8)));
	uint64_t		ready		__attribute__ ((aligned(8)));
} __attribute__ ((aligned(DEFAULT_CACHE_LINE_SIZE)));

__attribute__ ((aligned(DEFAULT_CACHE_LINE_SIZE)))
		static struct s_philo_event_internal	g_queue[EVENT_QUEUE_SIZE];
__attribute__ ((aligned(DEFAULT_CACHE_LINE_SIZE))) uint64_t g_head = 0; // to pop
uint64_t										g_tail = 0; // to push

void	enqueue(t_usec ts, enum e_status ev_type, size_t philo_id)
{
	size_t		my_tail;

	my_tail = fetch_add_mod(&g_tail, 1, EVENT_QUEUE_SIZE);
	while (g_queue[my_tail].ready);
	g_queue[my_tail].ts = ts;
	g_queue[my_tail].ev_type = ev_type;
	g_queue[my_tail].philo_id = philo_id;
	swap(&g_queue[my_tail].ready, 1);
}

struct s_philo_event	dequeue()
{
	struct s_philo_event	ret;
	volatile uint64_t		my_ready;

	my_ready = g_queue[g_head].ready;
	while (!my_ready)
	{
		usleep(10);
		my_ready = g_queue[g_head].ready;
	}
	ret.ts = g_queue[g_head].ts;
	ret.ev_type = g_queue[g_head].ev_type;
	ret.philo_id = g_queue[g_head].philo_id;
	compare_and_swap(&g_queue[g_head].ready, 1, 0);
	g_head = (g_head + 1) % EVENT_QUEUE_SIZE;
	return (ret);
}

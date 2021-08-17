#include "philo.h"
#include "event_queue.h"
#include "atomic_primitives.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct s_philo_event_internal
{
	t_usec ts				ALIGN_8;
	enum e_status ev_type	ALIGN_8;
	size_t philo_id			ALIGN_8;
	uint64_t ready			ALIGN_8;
} CPU_CACHE_ALIGN;

typedef struct s_philo_event_internal	t_philo_event_internal;

CPU_CACHE_ALIGN	t_philo_event_internal
								g_queue[EVENT_QUEUE_SIZE];
CPU_CACHE_ALIGN uint64_t		g_head = 0;
CPU_CACHE_ALIGN uint64_t		g_tail = 0;

void	ev_enqueue(t_usec ts, enum e_status ev_type, size_t philo_id)
{
	size_t		my_tail;

	my_tail = fetch_add(&g_tail, 1) % EVENT_QUEUE_SIZE;
	while (g_queue[my_tail].ready)
		usleep(75);
	g_queue[my_tail].ts = ts;
	g_queue[my_tail].ev_type = ev_type;
	g_queue[my_tail].philo_id = philo_id;
	swap(&g_queue[my_tail].ready, 1);
}

struct s_philo_event	ev_dequeue(void)
{
	struct s_philo_event	ret;
	volatile uint64_t		my_ready;

	my_ready = g_queue[g_head].ready;
	while (!my_ready)
	{
		usleep(250);
		my_ready = g_queue[g_head].ready;
	}
	ret.ts = g_queue[g_head].ts;
	ret.ev_type = g_queue[g_head].ev_type;
	ret.philo_id = g_queue[g_head].philo_id;
	compare_and_swap(&g_queue[g_head].ready, 1, 0);
	g_head = (g_head + 1) % EVENT_QUEUE_SIZE;
	return (ret);
}

void	ev_clean(void)
{
	memset(&g_queue, 0, sizeof g_queue);
	g_head = 0;
	g_tail = 0;
}

#include "helpers.h"
#include "philo.h"
#include "event_queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

t_context	g_context;

int	initialize_context(t_context *ctx, int argc, char **argv);

static void 	dump_event(struct s_philo_event* ev)
{
	const char *ev_type_string;

	if (ev->ev_type == Thinking)
		ev_type_string = "Thinking";
	else if (ev->ev_type == Eating)
		ev_type_string = "Eating";
	else if (ev->ev_type == Sleeping)
		ev_type_string = "Sleeping";
	else if (ev->ev_type == Died)
		ev_type_string = "Died";
	else
		return;
	printf("[%7d][%3zu][%15s]\n", ev->ts, ev->philo_id, ev_type_string);
}

static int 	detach_all()
{
	int 	i;

	i = 0;
	while (i < g_context.number_of_philos)
	{
		if (pthread_detach(g_context.philos[i]))
			return (EXIT_FAILURE);
		i++;
	}
	return (EXIT_SUCCESS);
}

/*
 * Philosofers:
 * ./philo \
 * 		num_of_philos \
 * 		time_to_die \
 * 		time_to_eat \
 * 		time_to_sleep \
 * 		[number_of_times_each_philosopher_must_eat]
 *
 */

int main(int argc, char **argv)
{
	struct s_philo_event	ev;

	memset(&ev, 0x0, sizeof ev);
	if (initialize_context(&g_context, argc, argv))
	{
		printf("Error on init context... :/\n");
		return (EXIT_FAILURE);
	}
	if (detach_all())
	{
		printf("Error on detaching threads... :/\n");
		return (EXIT_FAILURE);
	}
	while (ev.ev_type != Died)
	{
		ev = deque();
		dump_event(&ev);
	}
	return (EXIT_SUCCESS);
}

#include "helpers.h"
#include "philo.h"
#include "event_queue.h"
#include <stdio.h>
#include <stdbool.h>
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
//	uint64_t  test = 0;
//	int a = CAS(&test, 0, 1);
//	printf("new: %llu, %d\n", test, a);
//
//	t_fork  fork;
//	memset(&fork, 0, sizeof fork);
//	int res = fork_try_take(&fork); // CAS(&fork.locked, 0, 1);;
//
//	ms_usleep(100);
//	printf("res: %d, %llu\n", res, fork.locked);
//	return 0;

	int 	i;
	struct s_philo_event	ev;

	printf("Start\n");
	if (initialize_context(&g_context, argc, argv))
	{
		printf("Error on parsing arguments... :/\n");
		return (EXIT_FAILURE);
	}
	i = 0;
	while (i < g_context.number_of_philos)
	{
		pthread_detach(g_context.philos[i]);
		i++;
	}
	while (true)
	{
		ev = deque();
		dump_event(&ev);
		if (ev.ev_type == Died) break;
	}
	return (EXIT_SUCCESS);
}

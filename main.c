#include "helpers.h"
#include "philo.h"
#include "event_queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

t_context	g_context;

static void 	dump_event(struct s_philo_event* ev)
{
	const char *ev_type_string;
	const char *event_color = ANSI_COLOR_RESET;

	if (ev->ev_type == Thinking)
		ev_type_string = "Thinking";
	else if (ev->ev_type == Eating)
		ev_type_string = "Eating";
	else if (ev->ev_type == Sleeping)
		ev_type_string = "Sleeping";
	else if (ev->ev_type == Died)
	{
		event_color = ANSI_COLOR_RED;
		ev_type_string = "Died";
	}
	else
		return;
	printf("%4$s[%1$7d]%5$s[%2$3zu]%7$s[%3$15s]%6$s\n",
		ev->ts, ev->philo_id, ev_type_string,
		ANSI_COLOR_GREEN, ANSI_COLOR_YELLOW, ANSI_COLOR_RESET,
		event_color);
}

/*
 * Philosofers:
 * 	./philo \
 * 		num_of_philos \
 * 		time_to_die \
 * 		time_to_eat \
 * 		time_to_sleep \
 * 		[number_of_times_each_philosopher_must_eat]
 *
 */

int	initialize_context(t_context *ctx, int argc, char **argv);

int main(int argc, char **argv)
{
	struct s_philo_event	ev;

	memset(&ev, 0x0, sizeof ev);
	if (initialize_context(&g_context, argc, argv))
	{
		printf("Error on init context... :/\n");
		return (EXIT_FAILURE);
	}
	while (ev.ev_type != Died)
	{
		ev = deque();
		dump_event(&ev);
	}
	return (EXIT_SUCCESS);
}

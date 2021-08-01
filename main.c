#include "helpers.h"
#include "philo.h"
#include "event_queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

t_context	g_context;

/*
 * Philosophers:
 * 	./philo \
 * 		num_of_philos \
 * 		time_to_die \
 * 		time_to_eat \
 * 		time_to_sleep \
 * 		[number_of_times_each_philosopher_must_eat]
 *
 */

int			initialize_context(t_context *ctx, int argc, char **argv);
static void	dump_event(struct s_philo_event* ev);

int main(int argc, char **argv)
{
	struct s_philo_event	ev;

	memset(&ev, 0x0, sizeof ev);
	if (initialize_context(&g_context, argc, argv))
	{
		printf("Error on init context... :/\n");
		return (EXIT_FAILURE);
	}
	while (ev.ev_type != Died && ev.ev_type != AllAteNTimes)
	{
		ev = dequeue();
		dump_event(&ev);
	}
	return (EXIT_SUCCESS);
}

struct event_mapper {
	enum e_status	ev_type;
	const char		*ev_type_string;
	const char		*ev_type_color_string;
};

static const struct event_mapper evm[] = {
		  {Thinking, "Thinking", ANSI_COLOR_BLUE}
		, {Eating, "Eating", ANSI_COLOR_CYAN}
		, {Sleeping, "Sleeping", ANSI_COLOR_MAGENTA}
		, {Died, "Died", ANSI_COLOR_RED}
		, {AllAteNTimes,"All done", ANSI_COLOR_RED}
};

static void 	dump_event(struct s_philo_event* ev)
{
	const char *ev_type_string;
	const char *event_color = ANSI_COLOR_RESET;

	int i = 0;
	while (i < sizeof evm / sizeof evm[0])
	{
		if (ev->ev_type == evm[i].ev_type)
		{
			ev_type_string = evm[i].ev_type_string;
			event_color = evm[i].ev_type_color_string;
			break ;
		}
		i++;
	}
	printf("%4$s[%1$7d] %5$s[%2$3zu] %7$s[%3$10s]%6$s\n",
			ev->ts, ev->philo_id, ev_type_string,
			ANSI_COLOR_GREEN, ANSI_COLOR_YELLOW,
			ANSI_COLOR_RESET, event_color);
}

#include "helpers.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "philo.h"
#include "fork.h"

t_context	g_context;
int	initialize_context(t_context *ctx, int argc, char **argv);

void*	philo_life(void *a)
{
	t_philo_context		ctx;
	const size_t		id = (size_t)a;

	memset(&ctx, 0x0, sizeof ctx);
	printf("Philo alivem sizeof mtx: %zu!\n", sizeof (t_fork));

	while (true)
	{
		if (ctx.last_time_ate + g_context.time_to_die <= ctx.timestamp)
		{
			printf("[%7d][%2zu] died               (✖╭╮✖) \n", ctx.timestamp, id);
			return (NULL);
		}

		if (ctx.status == Sleeping
				&& ctx.end_of_current_action <= ctx.timestamp)
		{
			ctx.status = Thinking;
			printf("[%7d][%3zu] is thinking      ¯\\_(ツ)_/¯ \n", ctx.timestamp, id);
		}
		if (ctx.status == Eating
				&& ctx.end_of_current_action <= ctx.timestamp)
		{
			// release mutexes
			ctx.status = Sleeping;
			ctx.end_of_current_action = ctx.timestamp + g_context.time_to_sleep;
			printf("[%7d][%3zu] is sleeping      (ー。ー) ☽ \n", ctx.timestamp, id);
		}
		if (ctx.status == Thinking)
		{
			printf("[%7d][%3zu] has taken a fork ʕ∗•ڡ•∗ʔ ─∈ \n", ctx.timestamp, id);
			// acquire mutexes successfully
			ctx.status = Eating;
			ctx.last_time_ate = ctx.timestamp;
			ctx.end_of_current_action = ctx.timestamp + g_context.time_to_eat;
			printf("[%7d][%3zu] is eating        ( ˘▽˘)っ♨ \n", ctx.timestamp, id);
		}

		ms_usleep(DEFAULT_SLEEP_TIME_MS);
		ctx.timestamp += DEFAULT_SLEEP_TIME_MS;
	}
}



int main(int argc, char **argv)
{
	uint64_t  tochange = 18;
	int a = CAS(&tochange, 17, 19);

	printf("%llu, changed: %llu\n", tochange, a);

	return 0;
	int 	i;

	if (initialize_context(&g_context, argc, argv))
	{
		printf("Error on parsing arguments... :/\n");
		return (EXIT_FAILURE);
	}
	i = 0;
	while (i < g_context.number_of_philos)
	{
		pthread_join(g_context.philos[i], NULL);
		i++;
	}
}

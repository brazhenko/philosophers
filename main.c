#include "helpers.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/param.h>
#include "philo.h"
#include "fork.h"

t_context	g_context;
int	initialize_context(t_context *ctx, int argc, char **argv);

void 	philo_think(t_philo_context *ctx)
{
	const size_t	left_fork = ctx->id ? ctx->id - 1 : g_context.number_of_philos - 1;
	const size_t	right_fork = ctx->id;

	if (!fork_try_take(g_context.forks + MIN(left_fork, right_fork)))
		return;
	if (!fork_try_take(g_context.forks + MAX(left_fork, right_fork)))
	{
		fork_put_down(g_context.forks + MIN(left_fork, right_fork));
		return;
	}
	printf("[%7d][%3zu] has taken both forks ─∈ \n", ctx->timestamp, ctx->id);
	// acquire mutexes successfully
	ctx->status = Eating;
	ctx->last_time_ate = ctx->timestamp;
	ctx->end_of_current_action = ctx->timestamp + g_context.time_to_eat;
	printf("[%7d][%3zu] is eating        ( ˘▽˘)っ♨ \n", ctx->timestamp, ctx->id);
}

void	philo_eat(t_philo_context *ctx)
{
	const size_t	left_fork = ctx->id ? ctx->id - 1 : g_context.number_of_philos - 1;
	const size_t	right_fork = ctx->id;

	fork_put_down(g_context.forks + MAX(left_fork, right_fork));
	fork_put_down(g_context.forks + MIN(left_fork, right_fork));
	ctx->status = Sleeping;
	ctx->end_of_current_action = ctx->timestamp + g_context.time_to_sleep;
	printf("[%7d][%3zu] is sleeping      (ー。ー) ☽ \n", ctx->timestamp, ctx->id);
}

void	philo_sleep(t_philo_context *ctx)
{
	ctx->status = Thinking;
	printf("[%7d][%3zu] is thinking      ¯\\_(ツ)_/¯ \n", ctx->timestamp, ctx->id);
}

void 	philo_die(t_philo_context *ctx)
{
	printf("[%7d][%2zu] died               (✖╭╮✖) \n", ctx->timestamp, ctx->id);
}

void*	philo_life(void *a)
{
	t_philo_context		ctx;

	memset(&ctx, 0x0, sizeof ctx);
	ctx.id = (size_t)a;

	while (true)
	{
		if (ctx.last_time_ate + g_context.time_to_die <= ctx.timestamp)
		{
			philo_die(&ctx);
			return (NULL);
		}
		if (ctx.status == Sleeping && ctx.end_of_current_action <= ctx.timestamp)
			philo_sleep(&ctx);
		if (ctx.status == Eating && ctx.end_of_current_action <= ctx.timestamp)
			philo_eat(&ctx);
		if (ctx.status == Thinking)
			philo_think(&ctx);
		ms_usleep(DEFAULT_SLEEP_TIME_MS);
		ctx.timestamp += DEFAULT_SLEEP_TIME_MS;
	}
}

/*
 *
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

	printf("Start\n");
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

#include "helpers.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include "philo.h"
#include "fork.h"
#include "event_queue.h"
#include "atomic_primitives.h"
#include <pthread.h>

extern t_context	g_context;
void	philo_think(t_philo_context *ctx);
void	philo_eat(t_philo_context *ctx);
void	philo_sleep(t_philo_context *ctx);
void	philo_sync(t_philo_context *ctx);
void	philo_die(t_philo_context *ctx);

void	philo_sync(t_philo_context *ctx)
{
	const uint64_t	ts1 = g_context.forks[ctx->first_fork].data & FORK_TS;
	const uint64_t	ts2 = g_context.forks[ctx->second_fork].data & FORK_TS;

	if (ctx->status == Thinking
		&& ctx->last_time_ate + g_context.time_to_die <= ctx->timestamp
		&& ctx->first_fork == ctx->second_fork)
	{
		philo_die(ctx);
	}
	else if (ctx->status == Thinking
		&& ctx->last_time_ate + g_context.time_to_die <= ctx->timestamp)
	{
		if (ctx->last_time_ate + g_context.time_to_die < ts1
			|| ctx->last_time_ate + g_context.time_to_die < ts2)
		{
			ctx->timestamp = ctx->last_time_ate + g_context.time_to_die;
			philo_die(ctx);
		}
	}
	else if (ctx->status == Eating
		 && ctx->last_time_ate + g_context.time_to_die <= ctx->timestamp)
		philo_die(ctx);
	else if (ctx->status == Sleeping
		 && ctx->last_time_ate + g_context.time_to_die <= ctx->timestamp)
		philo_die(ctx);
}

void	philo_init(t_philo_context *ctx, size_t id)
{
	memset(ctx, 0x0, sizeof (t_philo_context));
	ctx->id = (size_t)id;
	ctx->label = (ctx->id & 0x1)
		 + ((ctx->id + 1 == g_context.number_of_philos
				&& (g_context.number_of_philos & 1)) << 1);
	if (g_context.number_of_philos == 1)
		ctx->label = 0;
	ctx->first_fork = MIN(ctx->id, (ctx->id + 1) % g_context.number_of_philos);
	ctx->second_fork = MAX(ctx->id, (ctx->id + 1) % g_context.number_of_philos);
	ctx->end_of_action = INT_MAX;
	ev_enqueue(ctx->timestamp, Thinking, ctx->id);
}

_Noreturn void	*philo_life(void *param)
{
	t_philo_context		ctx;

	philo_init(&ctx, (size_t)param);
	while (true)
	{
		philo_sync(&ctx);
		if (ctx.status == Sleeping && ctx.end_of_action <= ctx.timestamp)
			philo_sleep(&ctx);
		if (ctx.status == Thinking)
			philo_think(&ctx);
		if (ctx.status == Eating && ctx.end_of_action <= ctx.timestamp)
			philo_eat(&ctx);
		pthread_yield_np();
		ms_usleep(DEFAULT_SLEEP_TIME_MS);
		ctx.timestamp += DEFAULT_SLEEP_TIME_MS;
	}
}

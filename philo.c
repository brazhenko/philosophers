#include "helpers.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include "philo.h"
#include "fork.h"
#include "event_queue.h"

extern t_context	g_context;

void	philo_think(t_philo_context *ctx)
{
	uint32_t		ts1;
	uint32_t		ts2;

	if (!fork_try_take_ts_sync
			(g_context.forks + ctx->first_fork, &ts1, ctx->label))
		return ;
	if (!fork_try_take_ts_sync
			(g_context.forks + ctx->second_fork, &ts2, ctx->label))
	{
		fork_put_down(g_context.forks + ctx->first_fork);
		return ;
	}
	ctx->timestamp = MAX(MAX(ts1, ts2), ctx->last_time_awake);
	ctx->status = Eating;
	ctx->last_time_ate = ctx->timestamp;
	ctx->end_of_current_action = ctx->timestamp + g_context.time_to_eat;
	// This thread owns both forks, so we can write there (set end time)
	g_context.forks[ctx->first_fork].locked
			= g_context.forks[ctx->first_fork].locked & ~FORK_TS | ctx->end_of_current_action;
	g_context.forks[ctx->second_fork].locked
			= g_context.forks[ctx->second_fork].locked & ~FORK_TS | ctx->end_of_current_action;
	enqueue(ctx->timestamp, Eating, ctx->id);
	ctx->times_ate++;
	if (ctx->times_ate == g_context.time_to_eat && g_context.yes)
		;
}

void	philo_eat(t_philo_context *ctx)
{
	const size_t	left_fork = ctx->id;
	const size_t	right_fork = (ctx->id + 1) % g_context.number_of_philos;

	fork_put_down_ts_sync(g_context.forks + MAX(left_fork, right_fork),
			ctx->timestamp, MAX(left_fork, right_fork), g_context.number_of_philos);
	fork_put_down_ts_sync(g_context.forks + MIN(left_fork, right_fork),
			ctx->timestamp, MIN(left_fork, right_fork), g_context.number_of_philos);
	ctx->status = Sleeping;
	ctx->end_of_current_action = ctx->timestamp + g_context.time_to_sleep;
	enqueue(ctx->timestamp, Sleeping, ctx->id);
}

void	philo_sleep(t_philo_context *ctx)
{
	ctx->status = Thinking;
	ctx->last_time_awake = ctx->timestamp;
	enqueue(ctx->timestamp, Thinking, ctx->id);
}

void 	philo_die(t_philo_context *ctx)
{
	enqueue(ctx->timestamp, Died, ctx->id);
}

void 	philo_sync(t_philo_context *ctx)
{
	const uint64_t	ts1 = g_context.forks[ctx->first_fork].locked & FORK_TS;
	const uint64_t	ts2 = g_context.forks[ctx->second_fork].locked & FORK_TS;

	if (ctx->last_time_ate + g_context.time_to_die <= ctx->timestamp
		&&
		(
			ctx->last_time_ate + g_context.time_to_die < ts1
			|| ctx->last_time_ate + g_context.time_to_die < ts2
		))
	{
//		printf("ts1: %llu, ts2: %llu, myts: %u\n", ts1, ts2, ctx->timestamp);
		ctx->timestamp = MIN(ctx->timestamp, MAX(ts1, ts2));
		philo_die(ctx);
	}
}

_Noreturn void*	philo_life(void *a)
{
	t_philo_context		ctx;

	memset(&ctx, 0x0, sizeof ctx);
	ctx.id = (size_t)a;
	ctx.label = (ctx.id & 0x1)
			+ ((ctx.id + 1 == g_context.number_of_philos && (g_context.number_of_philos & 1)) << 1);
	ctx.first_fork = MIN(ctx.id,  (ctx.id + 1) % g_context.number_of_philos);
	ctx.second_fork = MAX(ctx.id,  (ctx.id + 1) % g_context.number_of_philos);

	enqueue(ctx.timestamp, Thinking, ctx.id);
	while (true)
	{
		philo_sync(&ctx);
		if (ctx.status == Sleeping && ctx.end_of_current_action <= ctx.timestamp)
			philo_sleep(&ctx);
		if (ctx.status == Thinking)
			philo_think(&ctx);
		if (ctx.status == Eating && ctx.end_of_current_action <= ctx.timestamp)
			philo_eat(&ctx);
		ms_usleep(DEFAULT_SLEEP_TIME_MS);
		ctx.timestamp += DEFAULT_SLEEP_TIME_MS;
	}
}

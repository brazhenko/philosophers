#include "helpers.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include "philo.h"
#include "fork.h"
#include "event_queue.h"

extern t_context	g_context;

void 	philo_think(t_philo_context *ctx)
{
	const size_t	left_fork = ctx->id;

	const size_t	right_fork
		= ctx->id == g_context.number_of_philos - 1 ? 0 : ctx->id + 1;
	uint32_t		ts1;
	uint32_t		ts2;

	if (!fork_try_take_ts_sync(g_context.forks + MIN(left_fork, right_fork),
							&ts1, ctx->label))
		return;
	if (!fork_try_take_ts_sync(g_context.forks + MAX(left_fork, right_fork),
							&ts2, ctx->label))
	{
		fork_put_down(g_context.forks + MIN(left_fork, right_fork));
		return;
	}
	// Syncing clocks
	ctx->timestamp = MAX(MAX(ts1, ts2), ctx->last_time_awake);
	ctx->status = Eating;
	ctx->last_time_ate = ctx->timestamp;
	ctx->end_of_current_action = ctx->timestamp + g_context.time_to_eat;
	enqueue(ctx->timestamp, Eating, ctx->id);
}

void	philo_eat(t_philo_context *ctx)
{
	const size_t	left_fork = ctx->id ? ctx->id - 1 : g_context.number_of_philos - 1;
	const size_t	right_fork = ctx->id;

	fork_put_down_ts_sync(g_context.forks + MAX(left_fork, right_fork),
					   ctx->timestamp, ctx->label, g_context.number_of_philos);
	fork_put_down_ts_sync(g_context.forks + MIN(left_fork, right_fork),
						  ctx->timestamp, ctx->label, g_context.number_of_philos);
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

void*	philo_life(void *a)
{
	t_philo_context		ctx;

	memset(&ctx, 0x0, sizeof ctx);
	ctx.id = (size_t)a;
	ctx.label = (ctx.id & 0x1)
			+ ((ctx.id + 1 == g_context.number_of_philos && (g_context.number_of_philos & 1)) << 1);
//	printf("id: %zu, label: %llu\n", ctx.id, ctx.label);

	enqueue(ctx.timestamp, Thinking, ctx.id);
	while (true)
	{
		if (ctx.last_time_ate + g_context.time_to_die <= ctx.timestamp)
		{
			philo_die(&ctx);
			return (NULL);
		}
//		printf("end: %u\n", ctx.end_of_current_action);
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

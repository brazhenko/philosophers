#include "philo.h"
#include "fork.h"
#include "event_queue.h"
#include "atomic_primitives.h"
#include <sys/param.h>
#include <stdlib.h>

extern t_context	g_context;

static int	philo_take_forks(uint32_t *ts1, uint32_t *ts2, t_philo_context *ctx)
{
	if (!fork_try_take_ts_sync(g_context.forks
			+ ctx->first_fork, ts1, ctx->label))
		return (EXIT_FAILURE);
	if (!fork_try_take_ts_sync(g_context.forks
			+ ctx->second_fork, ts2, ctx->label))
	{
		fork_put_down(g_context.forks + ctx->first_fork);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void	philo_think(t_philo_context *ctx)
{
	uint32_t		ts1;
	uint32_t		ts2;

	if (philo_take_forks(&ts1, &ts2, ctx))
		return ;
	ctx->timestamp = MAX(MAX(ts1, ts2), ctx->last_time_awake);
	ctx->status = Eating;
	ctx->last_time_ate = ctx->timestamp;
	ctx->end_of_action = ctx->timestamp + g_context.time_to_eat;
	g_context.forks[ctx->first_fork].data
		= (g_context.forks[ctx->first_fork].data & ~FORK_TS)
		| ctx->end_of_action;
	g_context.forks[ctx->second_fork].data
		= (g_context.forks[ctx->second_fork].data & ~FORK_TS)
		| ctx->end_of_action;
	ev_enqueue(ctx->timestamp, Eating, ctx->id);
	ctx->times_ate++;
	if (ctx->times_ate == g_context.number_of_times_each_philo_must_eat
		&& g_context.yes)
		if (g_context.number_of_philos
			== 1 + fetch_add(&g_context.number_of_philos_completed_eat_task, 1))
			ev_enqueue(ctx->timestamp, AllAteNTimes, ctx->id);
}

void	philo_eat(t_philo_context *ctx)
{
	fork_put_down_ts_sync(g_context.forks + ctx->second_fork,
		ctx->timestamp, ctx->second_fork, g_context.number_of_philos);
	fork_put_down_ts_sync(g_context.forks + ctx->first_fork,
		ctx->timestamp, ctx->first_fork, g_context.number_of_philos);
	ctx->status = Sleeping;
	ctx->end_of_action = ctx->timestamp + g_context.time_to_sleep;
	ev_enqueue(ctx->timestamp, Sleeping, ctx->id);
}

void	philo_sleep(t_philo_context *ctx)
{
	ctx->status = Thinking;
	ctx->last_time_awake = ctx->timestamp;
	ctx->end_of_action = INT_MAX;
	ev_enqueue(ctx->timestamp, Thinking, ctx->id);
}

void	philo_die(t_philo_context *ctx)
{
	ev_enqueue(ctx->timestamp, Died, ctx->id);
}

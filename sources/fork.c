#include "fork.h"
#include "atomic_primitives.h"
#include <stdint.h>

/*
** Turn is a state of t_fork mutex which defines
** what group of threads `label` can successfully
** call fork_try_take_ts_sync().
**
** To understand resource sharing strategy look at these diagram:
**
**                                               philo
**  Turn: 00                   Turn: 01          count    Turn: 10
**  (only even philos  anyway  (only odd philos  is odd   (the one 'extra' philo
**  are permitted to  ───────> are permitted    ────────> which is counted
**  take this fork,            to take fork)              neither odd nor even
**  default )                         |                   and has own permission
**      ▲                             │                   for lock)
**      │        philo_count is even  │              anyway   │
**      └─────────────────────────────┘───────────────────────┘
**
**
*/

/*
 * Resolves which `turn` state is next, i.e.
 * which thread group will be able to acquire
 * the t_fork mutex.
 *
 */
uint64_t	next_turn(
		uint64_t current_turn,
		uint64_t fork_id,
		size_t num_of_forks)
{
	if ((num_of_forks & 1) == 0)
		return (current_turn ^ 0x1);
	if (fork_id == 0)
		return (current_turn ^ 0x2);
	if (fork_id + 1 == num_of_forks)
		return (current_turn ^ 0x3);
	return (current_turn ^ 0x1);
}

int	fork_try_take(t_fork *fork)
{
	int				res;
	const uint64_t	data = fork->data;

	res = compare_and_swap(&fork->data, data, data | FORK_LOCKED);
	return (res);
}

void	fork_put_down(t_fork *fork)
{
	swap(&fork->data, fork->data & ~FORK_LOCKED);
}

int	fork_try_take_ts_sync(t_fork *fork, uint32_t *ts, uint64_t label)
{
	uint64_t	data;
	int			res;

	data = fork->data;
	if (data & FORK_LOCKED)
		return (0);
	if ((data >> FORK_TURN_SHIFT) != label)
		return (0);
	res = compare_and_swap(&fork->data, data, data | FORK_LOCKED);
	if (res)
		*ts = data & FORK_TS;
	return (res);
}

void	fork_put_down_ts_sync(t_fork *fork, uint32_t ts,
			uint64_t fork_id, size_t num_of_philos)
{
	const uint64_t	new_val = ts;

	swap(&fork->data,
		new_val
		| (next_turn(fork->data >> FORK_TURN_SHIFT,
				fork_id, num_of_philos) << FORK_TURN_SHIFT));
}

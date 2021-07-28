#include "fork.h"
#include <stdint.h>
#include <printf.h>

/*
 * 	Mapping
 * 	{ 00, 01, 10 | turn num one, two or three } * {0, 1 | count of philos is odd } \ { < 10,0 > } ->
 * 	-> { 00, 01, 10 | turn num one, two or three }
 *
 * 	To undersrand resource sharing strategy look at these diagram:
 *
 *                                                   philo
 * 	Turn: 00					Turn: 01			 count     Turn: 10
 * 	(only even philos   anyway	(only odd philos     is odd    (the one 'extra' philo
 * 	are permitted to   ───────> are permitted		────────>  which is counted neither
 * 	take this fork,				to take fork)                 nor even and has own
 * 	default )                          |                       permission for lock)
 *      ▲          philo_count is even │                 anyway   │
 *      └──────────────────────────────┘──────────────────────────┘
 *
 * 	Usage: g_next_turn[(turn << 1) | (philo_count & 1)]
 *
 */

__attribute__ ((aligned (DEFAULT_CACHELINE_SIZE))) static uint64_t	g_next_turn[8] = {
		0x1, 0x1, 0x0, 0x2, 0xFF, 0x0
};

int 	CAS(uint64_t *ptr, uint64_t old_val, uint64_t new_val)
{
	int	ret;
	asm	volatile (
		"mov %1, %%rax\n\t"
		"lock\n\t"
		"cmpxchg %2, (%3)\n\t"
		"mov $0, %0\n\t"
		"jnz 1f\n\t"
		"mov $1, %0\n\t"
		"1:\n\t"
		: "+r" (ret)
		: "r" (old_val), "r" (new_val), "r" (ptr)
		: "memory", "%rax"
	);
	return ret;
}

void 	S(uint64_t *ptr, uint64_t new_val)
{
	asm	volatile  (
		"lock\n\t"
		"xchg %0, (%1)\n\t"
		:: "r" (new_val), "r" (ptr)
		: "memory"
	);
}

int 	fork_try_take(t_fork *fork)
{
	const uint64_t data = fork->locked;

	if (data & FORK_LOCKED)
		return (0);
	return CAS(&fork->locked, data, data | FORK_LOCKED);
}

void 	fork_put_down(t_fork *fork)
{
//	printf("Put down\n");
	S(&fork->locked, fork->locked & ~FORK_LOCKED);
}

int 	fork_try_take_ts_sync(t_fork *fork, uint32_t *ts, uint64_t label)
{
	uint64_t	data;
	int 		res;

	data = fork->locked;
//	printf("data: %llx, %llx, label: %llx\n", data, data >> FORK_TURN_SHIFT, label);
	if (data & FORK_LOCKED)
		return (0);
	if ((data >> FORK_TURN_SHIFT) != label)
		return (0);
	res = CAS(&fork->locked, data, data | FORK_LOCKED);
//	printf("CAS: %d, 0x%010llx, label: %llu\n", res, fork->locked, label);
	if (res)
		*ts = data & FORK_TS;
	return res;
}

void 	fork_put_down_ts_sync(t_fork *fork, uint32_t ts, uint64_t label, size_t num_of_philos)
{
	const uint64_t	new_val = ts;

//	printf("new status 0x%010llx\n", new_val | (g_next_turn[label << 1 | (num_of_philos & 1)] << FORK_TURN_SHIFT));
	S(&fork->locked, new_val | (g_next_turn[label << 1 | (num_of_philos & 1)] << FORK_TURN_SHIFT));
}

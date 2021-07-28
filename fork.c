#include "fork.h"
#include <stdint.h>

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
		: "memory"
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
	S(&fork->locked, fork->locked & ~FORK_LOCKED);
}

int 	fork_try_take_ts_sync(t_fork *fork, uint32_t *ts)
{
	uint64_t	data;
	int 		res;

	data = fork->locked;
	if (data & FORK_LOCKED)
		return 0;
	res = CAS(&fork->locked, data, FORK_LOCKED);
	if (res)
		*ts = data & FORK_TS;
	return res;
}

void 	fork_put_down_ts_sync(t_fork *fork, uint32_t ts)
{
	const uint64_t	new_val = ts;

	S(&fork->locked, new_val);
}

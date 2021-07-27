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
	return CAS(&fork->locked, 0ULL, 1LL);
}

void 	fork_put_down(t_fork *fork)
{
	S(&fork->locked, 0);
}

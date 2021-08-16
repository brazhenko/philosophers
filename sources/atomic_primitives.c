#ifndef __x86_64__
# error "This implemetation is for x86_64 arch only"
#endif

#include <stdint.h>

int	compare_and_swap(uint64_t *ptr, uint64_t old_val, uint64_t new_val)
{
	int	ret;

	asm volatile (
		"mov %1, %%rax\n\t"
		"lock\n\t"
		"cmpxchg %2, (%3)\n\t"
		"mov $0, %0\n\t"
		"jnz 1f\n\t"
		"mov $1, %0\n\t"
		"1:\n\t"
		: "+r" (ret)
		: "r" (old_val), "r" (new_val), "r" (ptr)
		: "memory", "%rax");
	return (ret);
}

void	swap(uint64_t *ptr, uint64_t new_val)
{
	asm volatile (
		"lock\n\t"
		"xchg %0, (%1)\n\t"
		:: "r" (new_val), "r" (ptr)
		: "memory");
}

uint64_t	fetch_add(uint64_t *num, uint64_t to_add)
{
	uint64_t	ret;

	asm volatile (
		"lock xadd %0, (%1);"
		: "=a" (ret)
		: "r" (num), "a" (to_add)
		: "memory");
	return (ret);
}

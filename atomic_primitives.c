# include <stdint.h>

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

uint64_t	fetch_add_mod(uint64_t *num, uint64_t to_add, uint64_t mod)
{
	uint64_t	old_val;

	old_val = *num;
	while (!CAS(num, old_val, (old_val + to_add) % mod))
		old_val = *num;
	return old_val;
}

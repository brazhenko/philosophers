#include <stdint.h>

int 	CAS(uint64_t *ptr, uint64_t old_val, uint64_t new_val)
{
	int	ret = 0;

	asm	volatile  (
			"mov %rsi, %rax\n\t"
			"lock\n\t"
			"cmpxchg %rdx, (%rdi)\n\t"
			"popq    %rbp\n"
			"retq"
			);

//	return ret;
}

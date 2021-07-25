static int	CAS(int *ptr, int oldVal, int newVal)
{
	unsigned char	ret;
	__asm__	__volatile__(
	"  lock\n"
	"  cmpxchgl %[newval], %[mem]\n"
	"  sete %0\n"
	: "=q"			(ret)
	, [mem]"+m"		(*ptr)
	, "+a"			(oldVal)
	: [newval] "r"	(newVal)
	: "memory");

	return (ret);
}

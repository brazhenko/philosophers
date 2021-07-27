#ifndef FORK_H
# define FORK_H

# ifndef __APPLE__
#  error "Optimized for apple only"
# endif

# include <stdint.h>
# define DEFAULT_CACHELINE_SIZE 64

struct s_fork {
	uint64_t	locked;
} __attribute__ ((aligned (DEFAULT_CACHELINE_SIZE)));

typedef struct s_fork	t_fork;

int		CAS(uint64_t *ptr, uint64_t old_val, uint64_t new_val);
void 	S(uint64_t *ptr, uint64_t new_val);
int 	fork_try_take(t_fork *fork);
void 	fork_put_down(t_fork *fork);

#endif

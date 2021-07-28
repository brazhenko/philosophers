#ifndef FORK_H
# define FORK_H

# ifndef __APPLE__
#  error "Optimized for apple only"
# endif

# include <stdint.h>
# include <stddef.h>

# define DEFAULT_CACHELINE_SIZE 64
# define FORK_TURN				0x3000000000
# define FORK_TURN_00			0x0000000000
# define FORK_TURN_01			0x1000000000
# define FORK_TURN_10			0x2000000000
# define FORK_TURN_SHIFT		36ULL
# define FORK_LOCKED			0x0100000000ULL
# define FORK_TS				0x00FFFFFFFFULL


struct s_fork {
	uint64_t	locked;
} __attribute__ ((aligned (DEFAULT_CACHELINE_SIZE)));


typedef struct s_fork	t_fork;

int		CAS(uint64_t *ptr, uint64_t old_val, uint64_t new_val);
void 	S(uint64_t *ptr, uint64_t new_val);
int 	fork_try_take(t_fork *fork);
void 	fork_put_down(t_fork *fork);

int 	fork_try_take_ts_sync(t_fork *fork, uint32_t *ts, uint64_t label);
void 	fork_put_down_ts_sync(t_fork *fork, uint32_t ts, uint64_t label, size_t num_of_philos);

#endif

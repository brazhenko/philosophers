#ifndef FORK_H
# define FORK_H

# ifndef __APPLE__
#  error "Optimized for apple only"
# endif

# include "atomic_primitives.h"
# include <stdint.h>
# include <stddef.h>

# define FORK_TURN				0x3000000000
# define FORK_TURN_00			0x0000000000
# define FORK_TURN_01			0x1000000000
# define FORK_TURN_10			0x2000000000
# define FORK_TURN_SHIFT		36ULL
# define FORK_LOCKED			0x0100000000ULL
# define FORK_TS				0x00FFFFFFFFULL


struct s_fork {
	uint64_t	data;
} __attribute__ ((aligned (DEFAULT_CACHE_LINE_SIZE)));


typedef struct s_fork	t_fork;

int 	fork_try_take(t_fork *fork, uint32_t *ts_out, uint32_t ts_in, uint64_t label);
void 	fork_put_down(t_fork *fork);

int 	fork_try_take_ts_sync(t_fork *fork, uint32_t *ts, uint64_t label);
void 	fork_put_down_ts_sync(t_fork *fork, uint32_t ts, uint64_t label, size_t num_of_philos);

#endif

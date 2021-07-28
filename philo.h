#ifndef PHILO_H
# define PHILO_H

# include "fork.h"
# include <sys/time.h>
# include <stdbool.h>
# include <pthread.h>
# define DEFAULT_SLEEP_TIME_MS	10

enum e_status
{
	Thinking = 0,
	Eating,
	Sleeping,
	Died
};

typedef __darwin_suseconds_t t_usec;

typedef struct
{
	size_t	number_of_philos;
	t_usec	time_to_die;
	t_usec	time_to_eat;
	t_usec	time_to_sleep;
	bool	yes;
	size_t	number_of_times_each_philo_must_eat;

	pthread_t	*philos;
	t_fork 		*forks_real_ptr;
	t_fork		*forks;
}               t_context;

typedef struct
{
	size_t	id;
	t_usec	timestamp;
	t_usec	last_time_ate;
	t_usec	end_of_current_action;
	enum e_status	status;
}			t_philo_context;




#endif

#include "philo.h"
#include "fork.h"
#include "helpers.h"
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

_Noreturn void*		philo_life(void *a);

static int	initialize_philos(t_context *ctx)
{
	size_t		i;

	ctx->philos = (pthread_t *)malloc(sizeof (pthread_t) * ctx->number_of_philos);
	if (!ctx->philos)
	{
		printf("Error on allocating philos... exit :/\n");
		return (EXIT_FAILURE);
	}
	i = 0;
	while (i < ctx->number_of_philos)
	{
		if (pthread_create(&ctx->philos[i], NULL, philo_life, (void*)i))
		{
			printf("Error on creating philo: %zu... exit :/\n", i);
			return (EXIT_FAILURE);
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

static int 	detach_philos(t_context *ctx)
{
	int 	i;

	i = 0;
	while (i < ctx->number_of_philos)
	{
		if (pthread_detach(ctx->philos[i]))
			return (EXIT_FAILURE);
		i++;
	}
	return (EXIT_SUCCESS);
}

static int	initialize_forks(t_context *ctx)
{
	void *mem;
	const size_t	fork_real_alloc_size
		= sizeof (t_fork) * ctx->number_of_philos + DEFAULT_CACHELINE_SIZE;

	mem = malloc(fork_real_alloc_size);
	if (!mem)
		return (EXIT_FAILURE);
	ctx->forks_real_ptr = mem;
	ctx->forks = (t_fork *)
			(((uintptr_t)mem + (DEFAULT_CACHELINE_SIZE - 1)) & ~0x3f);
	memset(ctx->forks_real_ptr, 0x0, fork_real_alloc_size);
	if (ctx->number_of_philos & 1)
		ctx->forks[ctx->number_of_philos - 1].locked = FORK_TURN_01;
	return (EXIT_SUCCESS);
}



int initialize_context(t_context *ctx, int argc, char **argv)
{
	memset(ctx, 0x0, sizeof *ctx);
	if (argc < 5 || 6 < argc)
		return (EXIT_FAILURE);
	if (parse_size_t(argv[1], &ctx->number_of_philos)
		|| parse_size_t(argv[2], (size_t*)&ctx->time_to_die)
		|| parse_size_t(argv[3], (size_t*)&ctx->time_to_eat)
		|| parse_size_t(argv[4], (size_t*)&ctx->time_to_sleep))
		return (EXIT_FAILURE);
	if (argc > 6)
		return (EXIT_FAILURE);
	if (argc == 6)
	{
		if (parse_size_t(argv[5], &ctx->number_of_times_each_philo_must_eat))
			return (EXIT_FAILURE);
		ctx->yes = true;
	}
	if (initialize_forks(ctx))
		return (EXIT_FAILURE);
	printf("%s[%7s]%s[%3s]%s[%10s]%s\n",
		ANSI_COLOR_GREEN, "time",
		ANSI_COLOR_YELLOW, "id", ANSI_COLOR_RED, "event", ANSI_COLOR_RESET);
	if (initialize_philos(ctx))
		return (EXIT_FAILURE);
	if (detach_philos(ctx))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

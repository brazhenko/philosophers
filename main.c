#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

# define DELTA					715
# define DEFAULT_TS_UPDATE 		5000
# define DEFAULT_SLEEP_TIME_MCS	DEFAULT_TS_UPDATE - DELTA

typedef struct
{
    size_t	number_of_philos;
    size_t	time_to_die;
    size_t	time_to_eat;
    size_t	time_to_sleep;
    bool	yes;
	size_t	number_of_times_each_philo_must_eat;

	pthread_t	*philos;
	int 		*forks;
}               t_context;
t_context	context;


typedef __darwin_suseconds_t t_usec;

enum e_status
{
	Waiting = 0,
	Eating,
	Sleeping,
	Dead /* Unused */
};

typedef struct
{
	t_usec	timestamp;
	t_usec	last_time_ate;
	enum e_status	status;
}			t_philo_context;

bool is_digit(char c)
{
	return ('0' <= c && c <= '9');
}

int	parse_size_t(const char *str, size_t *out)
{
	int		i;

	*out = 0ULL;
	i = 0;
	while (str[i])
	{
		if (!is_digit(str[i]))
			return (EXIT_FAILURE);
		*out = *out * 10 + (str[i] - '0');
		i++;
	}
	return (EXIT_SUCCESS);
}


static int CAS(int *ptr, int oldVal, int newVal)
{
	unsigned char ret;

	__asm__ __volatile__ (
	"  lock\n"
	"  cmpxchgl %[newval], %[mem]\n"
	"  sete %0\n"
	: "=q" (ret), [mem] "+m" (*ptr), "+a" (oldVal)
	: [newval] "r" (newVal)
	: "memory"); // barrier for compiler reordering around this

	return ret; // ZF result, 1 on success else 0
}

void* philo_life(void *a)
{
	t_philo_context		ctx;

	memset(&ctx, 0x0, sizeof ctx);
	printf("Philo alive!\n");
	while (1)
	{
		if (ctx.last_time_ate + context.time_to_die * 1000 <= ctx.timestamp)
		{
			write(STDIN_FILENO, "die x_X\n", 8);
			return NULL;
		}
		if (ctx.status == Waiting)
		{
			// acquire mutexes
			ctx.status = Eating;
		}
		else if (ctx.status == Eating)
		{

			// release mutexes
			ctx.status = Sleeping;
		}
		else if (ctx.status == Sleeping)
		{
			ctx.status = Waiting;
		}

		usleep(DEFAULT_SLEEP_TIME_MCS);
		ctx.timestamp += DEFAULT_TS_UPDATE;
	}
}

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
		if (pthread_create(&ctx->philos[i], NULL, philo_life, NULL))
		{
			printf("Error on creating philo: %zu... exit :/\n", i);
			return (EXIT_FAILURE);
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

static int	initialize_forks(t_context *ctx)
{
	ctx->forks = (int *)malloc(sizeof (int) * ctx->number_of_philos);
	if (!ctx->forks)
		return (EXIT_FAILURE);
	memset(ctx->forks, 0x0, sizeof (int) * ctx->number_of_philos);
	return (EXIT_SUCCESS);
}


int initialize_context(t_context *ctx, int argc, char **argv)
{
	memset(ctx, 0x0, sizeof *ctx);
	if (argc < 5)
		return (EXIT_FAILURE);
	if (parse_size_t(argv[1], &ctx->number_of_philos))
		return (EXIT_FAILURE);
	if (parse_size_t(argv[2], &ctx->time_to_die))
		return (EXIT_FAILURE);
	if (parse_size_t(argv[3], &ctx->time_to_eat))
		return (EXIT_FAILURE);
	if (parse_size_t(argv[4], &ctx->time_to_sleep))
		return (EXIT_FAILURE);
	if (argc == 6)
	{
		if (parse_size_t(argv[5], &ctx->number_of_times_each_philo_must_eat))
			return (EXIT_FAILURE);
		ctx->yes = true;
	}
	if (initialize_philos(ctx))
		return (EXIT_FAILURE);
	if (initialize_forks(ctx))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}


int main(int argc, char **argv)
{
	int 	i;

	if (initialize_context(&context, argc, argv))
	{
		printf("Error on parsing arguments... :/\n");
		return (EXIT_FAILURE);
	}
	i = 0;
	while (i < context.number_of_philos)
	{
		pthread_join(context.philos[i], NULL);
		i++;
	}

}

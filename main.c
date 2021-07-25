#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

# define DEFAULT_SLEEP_TIME_MS	5

size_t	timeval_to_size_t(struct timeval timeval)
{
	return (timeval.tv_sec * 1000 + (size_t)(timeval.tv_usec * 0.001));
}

size_t	get_current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return (timeval_to_size_t(timeval));
}

void	ms_usleep(size_t sleep_time_in_ms)
{
	struct timeval	start_time_tv;
	size_t			start_time;

	gettimeofday(&start_time_tv, NULL);
	start_time = timeval_to_size_t(start_time_tv);
	while (1)
	{
		usleep(10);
		if (get_current_time() - start_time >= sleep_time_in_ms)
			return ;
	}
}
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
	int 		*forks;
}               t_context;
t_context	context;




enum e_status
{
	Thinking = 0,
	Eating,
	Sleeping,
	Dead /* Unused */
};

typedef struct
{
	t_usec	timestamp;
	t_usec	last_time_ate;
	t_usec	end_of_current_action;
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


static int	CAS(int *ptr, int oldVal, int newVal)
{
	unsigned char	ret;

	__asm__ __volatile__(
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
	const size_t		id = (size_t)a;

	memset(&ctx, 0x0, sizeof ctx);
	printf("Philo alive!\n");

	while (true)
	{
		if (ctx.last_time_ate + context.time_to_die <= ctx.timestamp)
		{
			printf("[%7d][%2zu] died               (✖╭╮✖) \n", ctx.timestamp, id);
			return (NULL);
		}

		if (ctx.status == Sleeping
				&& ctx.end_of_current_action <= ctx.timestamp)
		{
			ctx.status = Thinking;
			printf("[%7d][%3zu] is thinking      ¯\\_(ツ)_/¯ \n", ctx.timestamp, id);
		}
		if (ctx.status == Eating
				&& ctx.end_of_current_action <= ctx.timestamp)
		{
			// release mutexes
			ctx.status = Sleeping;
			ctx.end_of_current_action = ctx.timestamp + context.time_to_sleep;
			printf("[%7d][%3zu] is sleeping      (ー。ー) ☽ \n", ctx.timestamp, id);
		}
		if (ctx.status == Thinking)
		{
			printf("[%7d][%3zu] has taken a fork ʕ∗•ڡ•∗ʔ ─∈ \n", ctx.timestamp, id);
			// acquire mutexes successfully
			ctx.status = Eating;
			ctx.last_time_ate = ctx.timestamp;
			ctx.end_of_current_action = ctx.timestamp + context.time_to_eat;
			printf("[%7d][%3zu] is eating        ( ˘▽˘)っ♨ \n", ctx.timestamp, id);
		}

		ms_usleep(DEFAULT_SLEEP_TIME_MS);
		ctx.timestamp += DEFAULT_SLEEP_TIME_MS;
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
		if (pthread_create(&ctx->philos[i], NULL, philo_life, (void*)i))
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
	if (argc < 5 || 6 < argc)
		return (EXIT_FAILURE);
	if (parse_size_t(argv[1], &ctx->number_of_philos))
		return (EXIT_FAILURE);
	if (parse_size_t(argv[2], (size_t*)&ctx->time_to_die))
		return (EXIT_FAILURE);
	if (parse_size_t(argv[3], (size_t*)&ctx->time_to_eat))
		return (EXIT_FAILURE);
	if (parse_size_t(argv[4], (size_t*)&ctx->time_to_sleep))
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

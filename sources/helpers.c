#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

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

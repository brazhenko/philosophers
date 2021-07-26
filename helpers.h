#ifndef HELPERS_H
# define HELPERS_H

# include <stddef.h>
# include <stdbool.h>
# include <sys/time.h>

size_t	timeval_to_size_t(struct timeval timeval);
size_t	get_current_time(void);
void	ms_usleep(size_t sleep_time_in_ms);
bool	is_digit(char c);
int		parse_size_t(const char *str, size_t *out);


#endif

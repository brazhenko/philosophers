#ifndef HELPERS_H
# define HELPERS_H

# include <stddef.h>
# include <stdbool.h>
# include <sys/time.h>

# define ANSI_COLOR_RED     "\x1b[31;1m"
# define ANSI_COLOR_GREEN   "\x1b[32;1m"
# define ANSI_COLOR_YELLOW  "\x1b[33;1m"
# define ANSI_COLOR_BLUE    "\x1b[34;1m"
# define ANSI_COLOR_MAGENTA "\x1b[35;1m"
# define ANSI_COLOR_CYAN    "\x1b[36;1m"
# define ANSI_COLOR_WHITE   "\x1b[40;1m"
# define ANSI_COLOR_RESET   "\x1b[0m"

size_t	timeval_to_ms_integral(struct timeval timeval);
size_t	get_current_time(void);
void	ms_usleep(size_t sleep_time_in_ms);
bool	is_digit(char c);
int		parse_size_t(const char *str, size_t *out);

#endif

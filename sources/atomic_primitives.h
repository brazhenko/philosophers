#ifndef ATOMIC_PRIMITIVES_H
# define ATOMIC_PRIMITIVES_H

#ifndef __x86_64__
# error "This implemetation is for x86_64 arch only"
#endif

# include <stdint.h>
# define DEFAULT_CACHE_LINE_SIZE 64

/*
 * Proceeds compare-and-swap as follows:
 * ATOMICALLY {
 *     if (*ptr == old_val) {
 *         *ptr = new_val;
 *         return 1;
 *     }
 *     return 0;
 * }
 *
 * return: 1 success, 0 - failure
 *
 */
int			compare_and_swap(uint64_t *ptr, uint64_t old_val, uint64_t new_val);

/*
 * Proceeds swap as follows:
 * ATOMICALLY {
 *     *ptr = new_val;
 * }
 *
 */
void		swap(uint64_t *ptr, uint64_t new_val);

/*
 * Proceeds the following operation:
 * ATOMICALLY {
 *     old_value = *num;
 *     *num = (old_value + to_add) % mod;
 *     return old_value;
 * }
 *
 * return: old_value, value before change happened
 *
 */
uint64_t	fetch_add(uint64_t *num, uint64_t to_add);

#endif

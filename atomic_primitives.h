#ifndef ATOMIC_PRIMITIVES_H
# define ATOMIC_PRIMITIVES_H

# include <stdint.h>

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
int 	CAS(uint64_t *ptr, uint64_t old_val, uint64_t new_val);

/*
 * Proceeds swap as follows:
 * ATOMICALLY {
 *     *ptr = new_val;
 * }
 *
 */
void 	S(uint64_t *ptr, uint64_t new_val);

/*
 * Proceeds the following operation:
 * ATOMICALLY {
 *     old_value = *num;
 *     *num = (*num + to_add) % mod;
 *     return old_value;
 * }
 *
 * return: old_value, value before change happened
 *
 */
uint64_t	fetch_add_mod(uint64_t *num, uint64_t to_add, uint64_t mod);


#endif

// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup    ll_utils    LL utilities
 * @ingroup     ll
 * @brief       Logical link (LL) utilities, for internal use.
 */

#ifndef _LL_UTILS_H_
#define _LL_UTILS_H_

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * @ingroup ll_utils
 * @brief Log LL activity.
 * 
 * If the message is too verbose it is not printed, according to ll_config.verbosity
 * 
 * @param verbosity     Verbosity of the message (1 to 3)
 * @param format        Same as printf
 * @param ...           Same as printf
 * @return int          Same as printf, or 0 if too verbose
 */
int ll_log(int verbosity, const char *format, ...);

/**
 * @ingroup ll_utils
 * @brief Log LL errors.
 * 
 * @param format        Same as printf
 * @param ...           Same as printf
 * @return int          Same as printf
 */
int ll_err(const char *format, ...);

/**
 * @ingroup ll_utils
 * @brief Determine BCC byte value.
 * 
 * BCC is a checksum, composed from the XOR of all bytes in the segment one
 * wishes to check.
 * 
 * @param start     Pointer to start of array to calculate BCC
 * @param end       Pointer to past-the-end of array to calculate BCC
 * @return uint8_t  BCC byte
 */
uint8_t ll_bcc(const uint8_t *start, const uint8_t *end)
    __attribute__((warn_unused_result));

/**
 * @ingroup ll_utils
 * @brief Perform bit stuffing in an array of bytes.
 * 
 * Bits are stuffed using the escape character ESC (0x7D). This character is
 * used when a byte b is equal to FLAG (0x7E) or ESC (0x7D), which are encoded
 * as the ESC character followed by the result of the XOR between b and 0x20
 * (b ^ 0x20).
 * 
 * @param out       Output buffer; should be at least twice as large as the input buffer
 * @param in        Input buffer, where data is read from
 * @param length    Length of the input buffer
 * @return ssize_t  On success, the number of bytes written to the output
 *                  buffer; on error, -1, and errno is set
 * 
 * @see             ll_destuffing(uint8_t*, const uint8_t*, size_t)
 */
ssize_t ll_stuffing(uint8_t *out, const uint8_t *in, size_t length)
    __attribute__((warn_unused_result));

/**
 * @ingroup ll_utils
 * @brief Perform bit destuffing in an array of bytes.
 * 
 * @param out       Output buffer; should be at least as large as the input buffer
 * @param in        Input buffer, where data is read from
 * @param length    Length of the input buffer
 * @return ssize_t  On success, the number of bytes written to the output
 *                  buffer; on error, -1, and errno is set
 * 
 * @see             ll_stuffing(uint8_t*, const uint8_t*, size_t)
 */
ssize_t ll_destuffing(uint8_t *out, const uint8_t *in, size_t length)
    __attribute__((warn_unused_result));

#endif // _LL_UTILS_H_

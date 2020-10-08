#include <stdint.h>
#include <sys/types.h>

/**
 * @brief Determine BCC byte value.
 * 
 * BCC is a checksum, composed from the XOR of all bytes in the segment one wishes to check.
 * 
 * @param start     Pointer to start of array to calculate BCC
 * @param end       Pointer to past-the-end of array to calculate BCC
 * @return uint8_t  BCC byte
 */
uint8_t ll_bcc(const uint8_t *start, const uint8_t *end) __attribute__((warn_unused_result));

/**
 * @brief Perform bit stuffing in an array of bytes.
 * 
 * Bits are stuffed using the escape character ESC (0x7D). This character is used when a byte b is equal to
 * FLAG (0x7E) or ESC (0x7D), which are encoded as the ESC character followed by the result of the XOR
 * between b and 0x20 (b ^ 0x20).
 * 
 * @param out       Output buffer; should be at least twice as large as the input buffer
 * @param in        Input buffer, where data is read from
 * @param length    Length of the input buffer
 * @return ssize_t  On success, the number of bytes written to the output buffer; on error, -1, and errno is set
 * 
 * @see             ll_stuffing
 */
ssize_t ll_stuffing(uint8_t *out, const uint8_t *in, size_t length) __attribute__((warn_unused_result));

/**
 * @brief Perform bit destuffing in an array of bytes.
 * 
 * @param out       Output buffer; should be at least as large as the input buffer
 * @param in        Input buffer, where data is read from
 * @param length    Length of the input buffer
 * @return ssize_t  On success, the number of bytes written to the output buffer; on error, -1, and errno is set
 * 
 * @see             ssize_t ll_stuffing(uint8_t*, const uint8_t*, size_t)
 */
ssize_t ll_destuffing(uint8_t *out, const uint8_t *in, size_t length) __attribute__((warn_unused_result));

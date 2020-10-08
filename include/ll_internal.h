#ifndef _LL_INTERNAL_H_
#define _LL_INTERNAL_H_

#include <stdint.h>
#include <sys/types.h>
#include <termios.h>

#include "ll.h"

ll_status_t ll_status;
int timeout;
unsigned int sequence_number;

/**
 * @brief Handle alarm signal.
 * 
 * @param signum    Signal number (unused)
 */
void alarmHandler(__attribute__((unused)) int signum);

/**
 * @brief Get baud rate.
 * 
 * The baud rate can be specified as any integer in ll_config.baud_rate.
 * However, serial port drivers only support certain baud rates,
 * so this function takes the specified baud rate, rounds it up to the
 * next valid baud rate, and returns the corresponding baud rate flag
 * to use in termios.c_cflag.
 * 
 * @return tcflag_t     Valid baud rate flag for termios.c_cflag
 */
tcflag_t ll_get_baud_rate(void) __attribute__((warn_unused_result));

/**
 * @brief Get I-frame C byte expected from the other end of the communication.
 * 
 * @return uint8_t  Expected I-frame C byte
 */
uint8_t ll_get_expected_Iframe_C(void) __attribute__((warn_unused_result));

/**
 * @brief Get RR byte expected from the other end of the communication.
 * 
 * @return uint8_t  Expected RR byte
 */
uint8_t ll_get_expected_RR(void) __attribute__((warn_unused_result));

/**
 * @brief Get REJ byte expected from the other end of the communication.
 * 
 * @return uint8_t  Expected REJ byte
 */
uint8_t ll_get_expected_REJ(void) __attribute__((warn_unused_result));

/**
 * @brief Get I-frame C byte (according to the value of sequence_number, aka Ns).
 * 
 * @return uint8_t  I-frame C byte
 */
uint8_t ll_get_Iframe_C(void) __attribute__((warn_unused_result));

/**
 * @brief Send SET message (a S-frame) to serial port.
 * 
 * @param port_fd   Port to send SET message to
 * @return int      On success, the number of bytes written; on error, -1, and errno is set
 */
int ll_send_SET(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send DISC message (a S-frame) to serial port.
 * 
 * @param port_fd   Port to send DISC message to
 * @return int      On success, the number of bytes written; on error, -1, and errno is set
 */
int ll_send_DISC(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send UA message (a U-frame) to serial port.
 * 
 * @param port_fd   Port to send UA message to
 * @return int      On success, the number of bytes written; on error, -1, and errno is set
 */
int ll_send_UA(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send data message (I-frame) to serial port.
 * 
 * @param port_fd   Port to send data message to
 * @param buffer    Raw (destuffed) data to send to port
 * @param length    Length of data to send, in bytes
 * @return ssize_t  On success, the number of bytes written; on error, -1, and errno is set
 */
ssize_t ll_send_I(int port_fd, const uint8_t *buffer, size_t length) __attribute__((warn_unused_result));

/**
 * @brief Send Receiver Ready (RR) to port.
 * 
 * @param port_fd   Port to send RR to
 * @return int      0 if successful; -1 otherwise
 */
int ll_send_RR(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send Rejected (REJ) to port.
 * 
 * @param port_fd   Port to send REJ to
 * @return int      Number of bytes sent, if successful; -1 otherwise
 */
int ll_send_REJ(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Expect for S/U-frame to arrive from port.
 * 
 * @param port_fd   Port to expect the frame comes from
 * @param a_rcv     Pointer to memory where address (A) byte will be saved
 * @param c_rcv     Pointer to memory where control (C) byte will be saved
 * @return int      On success, 0; on error, another value, and errno is set
 */
int ll_expect_SUframe(int port_fd, uint8_t *a_rcv, uint8_t *c_rcv) __attribute__((warn_unused_result));

/**
 * @brief Expect for I-frame to arrive from port.
 * 
 * @param port_fd   Port to expect the frame comes from
 * @param buffer    Pointer to buffer where data will be saved
 * @return ssize_t  On success, number of characters read; on error, -1, and errno is set
 */
ssize_t ll_expect_Iframe(int port_fd, uint8_t *buffer) __attribute__((warn_unused_result));

#endif // _LL_INTERNAL_H_

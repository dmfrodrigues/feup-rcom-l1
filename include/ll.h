// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup    ll  LL
 * @brief       Logical link (LL) protocol.
 */

#ifndef _LL_H_
#define _LL_H_

#include <stdlib.h>
#include <sys/time.h>

#include "ll_flags.h"

#define LL_MAX_SIZE 4096    ///< @ingroup ll @brief Maximum message size

/**
 * @ingroup ll
 * @brief LL configuration structure.
 */
typedef struct {
    size_t baud_rate;               ///< Baud rate, in bit/s
    struct itimerval timeout;       ///< Timeout
    unsigned int retransmissions;   ///< Retransmissions
    int verbosity;                  ///< Verbosity, from 0 to 3
} ll_config_t;

/**
 * @ingroup ll
 * @brief LL status.
 */
typedef enum {
    TRANSMITTER,
    RECEIVER
} ll_status_t;

/**
 * @ingroup ll
 * @brief LL configuration object.
 */
ll_config_t ll_config;

/**
 * @ingroup ll
 * @brief Open serial port.
 * 
 * Since there is usually COM1-COM4, you should pass a value 1-4,
 * although you can use /dev/ttyS10 (COM11) by passing 11 as com.
 * 
 * @param com       Port to connect to.
 * @param status    Whether the caller is a transmitter or receiver
 * @return int      ID of connection, or a negative value if an error occured.
 */
int llopen(int com, ll_status_t status)
    __attribute__((warn_unused_result));

/**
 * @ingroup ll
 * @brief Write to serial port.
 * 
 * @param id        Port to write to.
 * @param buffer    Data to be written to port.
 * @param length    Length of data to be written to port.
 * @return int      Number of written characters, or a negative value if error.
 */
int llwrite(int id, const uint8_t *buffer, int length)
    __attribute__((warn_unused_result));

/**
 * @ingroup ll
 * @brief Read serial port.
 * 
 * @param id        Port to read.
 * @param buffer    Data to be read.
 * @return int      Number of read characters, or a negative value if error.
 */
int llread(int id, uint8_t *buffer)
    __attribute__((warn_unused_result));

/**
 * @ingroup ll
 * @brief Close serial port.
 * 
 * @param id        Port to close.
 * @return int      0 if successful, negative value otherwise
 */
int llclose(int id)
    __attribute__((warn_unused_result));

#endif //_LL_H_

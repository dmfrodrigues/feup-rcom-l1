// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup    ll_i_statemachine LL I-frame state machine
 * @ingroup     ll
 * @brief       Logical link (LL) state machine that allows
 *              to receive I-frames; for internal use.
 */

#ifndef _LL_I_STATEMACHINE_H_
#define _LL_I_STATEMACHINE_H_

#include <stdint.h>
#include <sys/types.h>

#include "ll.h"

/**
 * @ingroup ll_i_statemachine
 * @brief State of state machine to receive a I-frame.
 * 
 * I-frame: information frames
 */
typedef enum {
    LL_I_START,      ///< Start state
    LL_I_FLAG_RCV,   ///< Received flag
    LL_I_A_RCV,      ///< Received address
    LL_I_C_RCV,      ///< Received control byte
    LL_I_DATA,       ///< Received BCC1, and it is correct; going for data now
    LL_I_DATA_ESC,   ///< Received ESC, waiting data to escape
    LL_I_STOP,       ///< Stop (final) state
    LL_I_C_UNXP_RCV, ///< Unexpected C received (retransmission)
    LL_I_STOP_RR,    ///< Stop (final) state, should send RR
    LL_I_C_SET_RCV,  ///< Unexpected C received (SET)
    LL_I_STOP_UA     ///< Stop (final) state, should send UA
} ll_i_state_t;

/**
 * @ingroup ll_i_statemachine
 * @brief S/U-frame state machine.
 * 
 * Allows to read S-frames and U-frames.
 */
typedef struct {
    ll_i_state_t state;             ///< Machine state
//    uint8_t a_rcv;                  ///< Value of received Address (A) byte
//    uint8_t c_rcv;                  ///< Value of received Control (C) byte
    int     escaped;
    uint8_t data[2*LL_MAX_SIZE];    ///< Data in I-frame
    size_t  length;                 ///< Length of data
} ll_i_statemachine_t;

/**
 * @ingroup ll_i_statemachine
 * @brief Update state machine according to received byte.
 * 
 * @param machine   Pointer to state machine
 * @param byte      Byte received, will decide the transition
 * @return int      0 on success, other value otherwise
 */
int ll_i_state_update(ll_i_statemachine_t *machine, uint8_t byte)
    __attribute__((warn_unused_result));

#endif // _LL_I_STATEMACHINE_H_

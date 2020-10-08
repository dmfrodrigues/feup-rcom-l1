/**
 * @defgroup    ll_i_statemachine LL I-frame state machine
 * @ingroup     ll
 * @brief       Logical link (LL) state machine that allows to receive I-frames; for internal use.
 */

#ifndef _LL_SU_STATEMACHINE_H_
#define _LL_SU_STATEMACHINE_H_

#include <stdint.h>

#include "ll.h"

/**
 * @ingroup ll_i_statemachine
 * @brief State of state machine to receive a I-frame.
 * 
 * I-frame: information frames
 */
typedef enum {
    Start,      ///< Start state
    Flag_RCV,   ///< Received flag
    A_RCV,      ///< Received address
    C_RCV,      ///< Received control byte
    BCC_OK,     ///< Received BCC, and it is correct
    Stop        ///< Stop (final) state
} ll_i_state_t;

/**
 * @ingroup ll_i_statemachine
 * @brief S/U-frame state machine.
 * 
 * Allows to read S-frames and U-frames.
 */
typedef struct {
    ll_i_state_t state;             ///< Machine state
    uint8_t a_rcv;                  ///< Value of received Address (A) byte
    uint8_t c_rcv;                  ///< Value of received Control (C) byte
    uint8_t data[2*LL_MAX_SIZE];    ///< Data in I-frame
} ll_i_statemachine_t;

/**
 * @ingroup ll_i_statemachine
 * @brief Update state machine according to received byte.
 * 
 * @param machine   Pointer to state machine
 * @param byte      Byte received, will decide the transition
 * @return int      0 on success, other value otherwise
 */
int ll_i_state_update(ll_i_statemachine_t *machine, uint8_t byte) __attribute__((warn_unused_result));

#endif
/**
 * @defgroup    ll_s_statemachine LL S/U-frame state machine
 * @ingroup     ll
 * @brief       Logical link (LL) state machine that allows
 *              to receive S-frames and U-frames; for internal use.
 */

#ifndef _LL_S_STATEMACHINE_H_
#define _LL_S_STATEMACHINE_H_

#include <stdint.h>

/**
 * @ingroup ll_s_statemachine
 * @brief State of state machine to receive a S-frame or a U-frame.
 * 
 * S-frame: supervisory frames
 * U-frame: unnumbered frames
 */
typedef enum {
    LL_S_START,      ///< Start state
    LL_S_FLAG_RCV,   ///< Received flag
    LL_S_A_RCV,      ///< Received address
    LL_S_C_RCV,      ///< Received control byte
    LL_S_BCC_OK,     ///< Received BCC, and it is correct
    LL_S_STOP,       ///< Stop (final) state
    LL_S_C_I_RCV,    ///< Received unexpected I-frame
    LL_S_STOP_RR     ///< Stop (final) state, send RR
} ll_s_state_t;

/**
 * @ingroup ll_s_statemachine
 * @brief S/U-frame state machine.
 * 
 * Allows to read S-frames and U-frames.
 */
typedef struct {
    ll_s_state_t state;    ///< Machine state
    uint8_t a_rcv;          ///< Value of received Address (A) byte
    uint8_t c_rcv;          ///< Value of received Control (C) byte
} ll_s_statemachine_t;

/**
 * @ingroup ll_s_statemachine
 * @brief Update state machine according to received byte.
 * 
 * @param machine   Pointer to state machine
 * @param byte      Byte received, will decide the transition
 * @return int      0 on success, other value otherwise
 */
int ll_s_state_update(ll_s_statemachine_t *machine, uint8_t byte)
    __attribute__((warn_unused_result));

#endif // _LL_S_STATEMACHINE_H_
/**
 * @defgroup    ll_u_statemachine LL U-frame state machine
 * @ingroup     ll
 * @brief       Logical link (LL) state machine that allows
 *              to receive U-frames; for internal use.
 */

#ifndef _LL_U_STATEMACHINE_H_
#define _LL_U_STATEMACHINE_H_

#include <stdint.h>

/**
 * @ingroup ll_u_statemachine
 * @brief State of state machine to receive aU-frame.
 * 
 * S-frame: supervisory frames
 * U-frame: unnumbered frames
 */
typedef enum {
    LL_U_START,      ///< Start state
    LL_U_FLAG_RCV,   ///< Received flag
    LL_U_A_RCV,      ///< Received address
    LL_U_C_RCV,      ///< Received control byte
    LL_U_BCC_OK,     ///< Received BCC, and it is correct
    LL_U_STOP,       ///< Stop (final) state
    LL_U_C_DISC_RCV, ///< Received unexpected SET (retransmission from establishment)
    LL_U_STOP_DISC   ///< Stop (final) state, send DISC
} ll_u_state_t;

/**
 * @ingroup ll_u_statemachine
 * @brief U-frame state machine.
 * 
 * Allows to read S-frames and U-frames.
 */
typedef struct {
    ll_u_state_t state;    ///< Machine state
    uint8_t a_rcv;          ///< Value of received Address (A) byte
    uint8_t c_rcv;          ///< Value of received Control (C) byte
} ll_u_statemachine_t;

/**
 * @ingroup ll_u_statemachine
 * @brief Update state machine according to received byte.
 * 
 * @param machine   Pointer to state machine
 * @param byte      Byte received, will decide the transition
 * @return int      0 on success, other value otherwise
 */
int ll_u_state_update(ll_u_statemachine_t *machine, uint8_t byte)
    __attribute__((warn_unused_result));

#endif // _LL_U_STATEMACHINE_H_
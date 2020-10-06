#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include <stdint.h>

/**
 * State of state machine to receive a S-frame or a U-frame.
 * S-frame: supervisory frames
 * U-frame: unnumbered frames
 */
typedef enum {
    Start,      // Start state
    Flag_RCV,   // Received flag
    A_RCV,      // Received address
    C_RCV,      // Received control byte
    BCC_OK,     // Received BCC, and it is correct
    Stop        // W
} ll_su_state_t;

uint8_t sm_a_rcv, sm_c_rcv;

ll_su_state_t ll_su_state_update(ll_su_state_t state, uint8_t byte) __attribute__((warn_unused_result));

#endif
#ifndef _LL_SU_STATEMACHINE_H_
#define _LL_SU_STATEMACHINE_H_

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

typedef struct {
    ll_su_state_t state;
    uint8_t a_rcv, c_rcv;
} ll_su_statemachine_t;

int ll_su_state_update(ll_su_statemachine_t *machine, uint8_t byte) __attribute__((warn_unused_result));

#endif
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include <stdint.h>

typedef enum {
    Start,      // Start state
    Flag_RCV,   // Received flag
    A_RCV,      // Received address
    C_RCV,      // Received control byte
    BCC_OK,     // Received BCC, and it is correct
    Stop        // W
} stateMachine;

stateMachine updateStateMachine(stateMachine state, uint8_t byte);

#endif
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

// [FLAG,A,C,BCC,FLAG]
// flag = 01111110; A=11; C=11; BCC = A XOR C 
#define FLAG 0x7E
#define A 0x03
#define C 0x03
#define BCC (A^C)

typedef enum {
    Start,      // Start state
    Flag_RCV,   // Received flag
    A_RCV,      // Received address
    C_RCV,      // Received control byte
    BCC_OK,     // Received BCC, and it is correct
    Stop        // W
} stateMachine;

stateMachine updateStateMachine(stateMachine state, char byte);

#endif
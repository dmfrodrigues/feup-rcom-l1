#include <stdio.h>
#include "statemachine.h"

char a_rcv, c_rcv;

stateMachine updateStateMachine(stateMachine state, char byte){
    switch (state) {
    case Start:
        switch(byte){
            case FLAG:               state = Flag_RCV; break;
            default  :               state = Start   ; break;
        } break;
    case Flag_RCV:
        switch(byte){
            case A   : a_rcv = byte; state = A_RCV   ; break;
            case FLAG:               state = Flag_RCV; break;
            default  :               state = Start   ; break;
        } break;
    case A_RCV:
        switch(byte){
            case C   : c_rcv = byte; state = C_RCV   ; break;
            case FLAG:               state = Flag_RCV; break;
            default  :               state = Start   ; break;
        } break;
    case C_RCV:
        switch(byte){
            case FLAG: state = Flag_RCV; break;
            default  : state = (byte == (a_rcv^c_rcv) ? BCC_OK : Start); break;
        } break;
    case BCC_OK:
        switch(byte){
            case FLAG: state = Stop ; break;
            default  : state = Start; break;
        } break;
    case Stop:
        state = Stop;
        break;
    default:
        fprintf(stderr, "No such state %d\n", state);
        break;
    }
    fprintf(stderr, "(debug) STATE: %d \n", state);
    return state;
}

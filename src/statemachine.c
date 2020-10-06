#include <stdio.h>
#include "statemachine.h"

#include "flags.h"

su_state_t update_su_state(su_state_t state, uint8_t byte){
    switch (state) {
    case Start:
        switch(byte){
            case SP_FLAG  :               state = Flag_RCV; break;
            default       :               state = Start   ; break;
        } break;
    case Flag_RCV:
        switch(byte){
            case SP_A_SEND:
            case SP_A_RECV: a_rcv = byte; state = A_RCV   ; break;
            case SP_FLAG  :               state = Flag_RCV; break;
            default       :               state = Start   ; break;
        } break;
    case A_RCV:
        switch(byte){
            case SP_C_SET :
            case SP_C_DISC:
            case SP_C_UA  : c_rcv = byte; state = C_RCV   ; break;
            case SP_FLAG  :               state = Flag_RCV; break;
            default       :               state = Start   ; break;
        } break;
    case C_RCV:
        switch(byte){
            case SP_FLAG  : state = Flag_RCV; break;
            default       : state = (byte == (a_rcv^c_rcv) ? BCC_OK : Start); break;
        } break;
    case BCC_OK:
        switch(byte){
            case SP_FLAG  : state = Stop ; break;
            default       : state = Start; break;
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

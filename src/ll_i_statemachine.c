#include "ll_i_statemachine.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_flags.h"

int ll_i_state_update(ll_i_statemachine_t *machine, uint8_t byte){
    switch (machine->state) {
    case Start:
        switch(byte){
            case LL_FLAG  :                        machine->state = Flag_RCV; break;
            default       :                        machine->state = Start   ; break;
        } break;
    case Flag_RCV:
        switch(byte){
            case LL_A_SEND:
            case LL_A_RECV: machine->a_rcv = byte; machine->state = A_RCV   ; break;
            case LL_FLAG  :                        machine->state = Flag_RCV; break;
            default       :                        machine->state = Start   ; break;
        } break;
    case A_RCV:
        switch(byte){
            case LL_C_SET :
            case LL_C_DISC:
            case LL_C_UA  : machine->c_rcv = byte; machine->state = C_RCV   ; break;
            case LL_FLAG  :                        machine->state = Flag_RCV; break;
            default       :                        machine->state = Start   ; break;
        } break;
    case C_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = Flag_RCV; break;
            default       : machine->state = (byte == (machine->a_rcv^machine->c_rcv) ? BCC_OK : Start); break;
        } break;
    case BCC_OK:
        switch(byte){
            case LL_FLAG  : machine->state = Stop ; break;
            default       : machine->state = Start; break;
        } break;
    case Stop:
        machine->state = Stop;
        break;
    default:
        fprintf(stderr, "No such state %d\n", machine->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#include "ll_su_statemachine.h"

#include <stdio.h>
#include <stdlib.h>

#include "flags.h"

int ll_su_state_update(ll_su_statemachine_t *machine, uint8_t byte){
    switch (machine->state) {
    case Start:
        switch(byte){
            case SP_FLAG  :                        machine->state = Flag_RCV; break;
            default       :                        machine->state = Start   ; break;
        } break;
    case Flag_RCV:
        switch(byte){
            case SP_A_SEND:
            case SP_A_RECV: machine->a_rcv = byte; machine->state = A_RCV   ; break;
            case SP_FLAG  :                        machine->state = Flag_RCV; break;
            default       :                        machine->state = Start   ; break;
        } break;
    case A_RCV:
        switch(byte){
            case SP_C_SET :
            case SP_C_DISC:
            case SP_C_UA  : machine->c_rcv = byte; machine->state = C_RCV   ; break;
            case SP_FLAG  :                        machine->state = Flag_RCV; break;
            default       :                        machine->state = Start   ; break;
        } break;
    case C_RCV:
        switch(byte){
            case SP_FLAG  : machine->state = Flag_RCV; break;
            default       : machine->state = (byte == (machine->a_rcv^machine->c_rcv) ? BCC_OK : Start); break;
        } break;
    case BCC_OK:
        switch(byte){
            case SP_FLAG  : machine->state = Stop ; break;
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

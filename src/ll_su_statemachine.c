#include "ll_su_statemachine.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_flags.h"

int ll_su_state_update(ll_su_statemachine_t *machine, uint8_t byte){
    switch (machine->state) {
    case LL_SU_Start:
        switch(byte){
            case LL_FLAG  :                        machine->state = LL_SU_Flag_RCV; break;
            default       :                        machine->state = LL_SU_Start   ; break;
        } break;
    case LL_SU_Flag_RCV:
        switch(byte){
            case LL_A_SEND:
            case LL_A_RECV: machine->a_rcv = byte; machine->state = LL_SU_A_RCV   ; break;
            case LL_FLAG  :                        machine->state = LL_SU_Flag_RCV; break;
            default       :                        machine->state = LL_SU_Start   ; break;
        } break;
    case LL_SU_A_RCV:
        switch(byte){
            case LL_C_SET :
            case LL_C_DISC:
            case LL_C_UA  : machine->c_rcv = byte; machine->state = LL_SU_C_RCV   ; break;
            case LL_FLAG  :                        machine->state = LL_SU_Flag_RCV; break;
            default       :                        machine->state = LL_SU_Start   ; break;
        } break;
    case LL_SU_C_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = LL_SU_Flag_RCV; break;
            default       : machine->state = (byte == (machine->a_rcv^machine->c_rcv) ? LL_SU_BCC_OK : LL_SU_Start); break;
        } break;
    case LL_SU_BCC_OK:
        switch(byte){
            case LL_FLAG  : machine->state = LL_SU_Stop ; break;
            default       : machine->state = LL_SU_Start; break;
        } break;
    case LL_SU_Stop:
        fprintf(stderr, "can't transition from LL_SU_Stop\n");
        return EXIT_FAILURE;
    default:
        fprintf(stderr, "No such state %d\n", machine->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

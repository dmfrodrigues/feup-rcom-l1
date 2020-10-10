#include "ll_s_statemachine.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_flags.h"
#include "ll_internal.h"

int ll_s_state_update(ll_s_statemachine_t *machine, uint8_t byte){
    switch (machine->state) {
    case LL_S_START:
        switch(byte){
            case LL_FLAG  :                        machine->state = LL_S_FLAG_RCV; break;
            default       :                        machine->state = LL_S_START   ; break;
        } break;
    case LL_S_FLAG_RCV:
        switch(byte){
            case LL_A_SEND:
            case LL_A_RECV: machine->a_rcv = byte; machine->state = LL_S_A_RCV   ; break;
            case LL_FLAG  :                        machine->state = LL_S_FLAG_RCV; break;
            default       :                        machine->state = LL_S_START   ; break;
        } break;
    case LL_S_A_RCV:
        switch(byte){
            case LL_C_SET :
            case LL_C_DISC:
            case LL_C_UA  : machine->c_rcv = byte; machine->state = LL_S_C_RCV   ; break;
            case LL_FLAG  :                        machine->state = LL_S_FLAG_RCV; break;
            default       :
                if(byte == ll_get_expected_RR() || byte == ll_get_expected_REJ()) { machine->c_rcv = byte; machine->state = LL_S_C_RCV; }
                else machine->state = LL_S_START;
                break;
        } break;
    case LL_S_C_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = LL_S_FLAG_RCV; break;
            default       : machine->state = (byte == (machine->a_rcv^machine->c_rcv) ? LL_S_BCC_OK : LL_S_START); break;
        } break;
    case LL_S_BCC_OK:
        switch(byte){
            case LL_FLAG  : machine->state = LL_S_STOP ; break;
            default       : machine->state = LL_S_START; break;
        } break;
    case LL_S_STOP:
        fprintf(stderr, "can't transition from LL_S_STOP\n");
        return EXIT_FAILURE;
    default:
        fprintf(stderr, "No such state %d\n", machine->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#include "ll_i_statemachine.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ll.h"
#include "ll_internal.h"

int ll_i_state_update(ll_i_statemachine_t *machine, uint8_t byte){
    switch (machine->state) {
    case LL_I_START:
        switch(byte){
            case LL_FLAG  : machine->state = LL_I_FLAG_RCV; break;
            default       : machine->state = LL_I_START   ; break;
        } break;
    case LL_I_FLAG_RCV:
        switch(byte){
            case LL_A_SEND: machine->state = LL_I_A_RCV   ; break;
            case LL_FLAG  : machine->state = LL_I_FLAG_RCV; break;
            default       : machine->state = LL_I_START   ; break;
        } break;
    case LL_I_A_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = LL_I_FLAG_RCV; break;
            default       :
                if     (byte == ll_get_expected_Iframe_C  ()) machine->state = LL_I_C_RCV;
                else if(byte == ll_get_unexpected_Iframe_C()) machine->state = LL_I_C_UNXP_RCV;
                else if(byte == LL_C_SET                    ) machine->state = LL_I_C_SET_RCV;
                else                                          machine->state = LL_I_START;
                break;
        } break;
    case LL_I_C_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = LL_I_FLAG_RCV; break;
            default       : machine->state = (byte == (LL_A_SEND^ll_get_expected_Iframe_C()) ? LL_I_DATA : LL_I_START); break;
        } break;
    case LL_I_DATA:
        switch(byte){
            case LL_ESC   : machine->state = LL_I_DATA_ESC; machine->escaped = true; break;
            case LL_FLAG  : machine->state = LL_I_STOP    ; break;
            default       : machine->data[machine->length++] = byte; break;
        } break;
    case LL_I_DATA_ESC:
        switch(byte){
            case LL_ESC   :
            case LL_FLAG  : return EXIT_FAILURE;
            default       : machine->data[machine->length++] = LL_STUFF(byte); break;
        } break;
    case LL_I_STOP:
        fprintf(stderr, "can't transition from LL_I_STOP\n");
        return EXIT_FAILURE;
    case LL_I_C_UNXP_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = LL_I_STOP_RR; break;
            default       : machine->state = LL_I_C_UNXP_RCV; break;
        }
        break;
    case LL_I_STOP_RR:
        fprintf(stderr, "can't transition from LL_I_STOP_RR\n");
        return EXIT_FAILURE;
    case LL_I_C_SET_RCV:
        switch(byte){
            case LL_FLAG  : machine->state = LL_I_STOP_UA; break;
            default       : machine->state = LL_I_C_SET_RCV; break;
        }
        break;
    case LL_I_STOP_UA:
        fprintf(stderr, "can't transition from LL_I_STOP_UA\n");
        return EXIT_FAILURE;
    default:
        fprintf(stderr, "No such state %d\n", machine->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

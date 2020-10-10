#include "ll_u_statemachine.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_flags.h"
#include "ll_internal.h"

int ll_u_state_update(ll_u_statemachine_t *m, uint8_t b){
    switch (m->state) {
    case LL_U_START:
        switch(b){
            case LL_FLAG  :               m->state = LL_U_FLAG_RCV; break;
            default       :               m->state = LL_U_START   ; break;
        } break;
    case LL_U_FLAG_RCV:
        switch(b){
            case LL_A_SEND:
            case LL_A_RECV: m->a_rcv = b; m->state = LL_U_A_RCV   ; break;
            case LL_FLAG  :               m->state = LL_U_FLAG_RCV; break;
            default       :               m->state = LL_U_START   ; break;
        } break;
    case LL_U_A_RCV:
        switch(b){
            case LL_C_UA  : m->c_rcv = b; m->state = LL_U_C_RCV   ; break;
            case LL_C_DISC: m->c_rcv = b; m->state = LL_U_C_DISC_RCV; break;
            case LL_FLAG  :               m->state = LL_U_FLAG_RCV; break;
            default       :               m->state = LL_U_START   ; break;
        } break;
    case LL_U_C_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_U_FLAG_RCV; break;
            default       :
                m->state = (b == (m->a_rcv^m->c_rcv) ? LL_U_BCC_OK : LL_U_START);
                break;
        } break;
    case LL_U_BCC_OK:
        switch(b){
            case LL_FLAG  : m->state = LL_U_STOP ; break;
            default       : m->state = LL_U_START; break;
        } break;
    case LL_U_STOP:
        fprintf(stderr, "can't transition from LL_U_STOP\n");
        return EXIT_FAILURE;
    case LL_U_C_DISC_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_U_STOP_DISC; break;
            default       : m->state = LL_U_C_DISC_RCV; break;
        } break;
    case LL_U_STOP_DISC:
        fprintf(stderr, "can't transition from LL_U_STOP_DISC\n");
        return EXIT_FAILURE;
    default:
        fprintf(stderr, "No such state %d\n", m->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

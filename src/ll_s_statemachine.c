// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "ll_s_statemachine.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll_flags.h"
#include "ll_internal.h"

int ll_s_state_update(ll_s_statemachine_t *m, uint8_t b){
    switch (m->state) {
    case LL_S_START:
        switch(b){
            case LL_FLAG  :                  m->state = LL_S_FLAG_RCV; break;
            default       :                  m->state = LL_S_START   ; break;
        } break;
    case LL_S_FLAG_RCV:
        switch(b){
            case LL_A_SEND:
            case LL_A_RECV: m->a_rcv = b; m->state = LL_S_A_RCV   ; break;
            case LL_FLAG  :               m->state = LL_S_FLAG_RCV; break;
            default       :               m->state = LL_S_START   ; break;
        } break;
    case LL_S_A_RCV:
        switch(b){
            case LL_C_SET :
            case LL_C_DISC: m->c_rcv = b; m->state = LL_S_C_RCV   ; break;
            case LL_FLAG  :               m->state = LL_S_FLAG_RCV; break;
            default       :
                if     (b == ll_get_expected_RR() || b == ll_get_expected_REJ())
                    { m->c_rcv = b; m->state = LL_S_C_RCV; }
                else if(b==ll_get_expected_Iframe_C() || b==ll_get_unexpected_Iframe_C())
                    { m->c_rcv = b; m->state = LL_S_C_I_RCV; }
                else m->state = LL_S_START;
                break;
        } break;
    case LL_S_C_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_S_FLAG_RCV; break;
            default: m->state = (b == (m->a_rcv^m->c_rcv)?LL_S_BCC_OK:LL_S_START); break;
        } break;
    case LL_S_BCC_OK:
        switch(b){
            case LL_FLAG  : m->state = LL_S_STOP ; break;
            default       : m->state = LL_S_START; break;
        } break;
    case LL_S_STOP:
        ll_err("ERROR: can't transition LL_S_STOP\n");
        return EXIT_FAILURE;
    case LL_S_C_I_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_S_STOP_RR; break;
            default       : m->state = LL_S_C_I_RCV; break;
        } break;
    case LL_S_STOP_RR:
        ll_err("ERROR: can't transition LL_S_STOP_RR\n");
        return EXIT_FAILURE;
    default:
        ll_err("ERROR: No such state %d\n", m->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

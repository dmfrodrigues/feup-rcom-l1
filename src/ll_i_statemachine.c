// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "ll_i_statemachine.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ll.h"
#include "ll_internal.h"

int ll_i_state_update(ll_i_statemachine_t *m, uint8_t b){
    switch (m->state) {
    case LL_I_START:
        switch(b){
            case LL_FLAG  : m->state = LL_I_FLAG_RCV; break;
            default       : m->state = LL_I_START   ; break;
        } break;
    case LL_I_FLAG_RCV:
        switch(b){
            case LL_A_SEND: m->state = LL_I_A_RCV   ; break;
            case LL_FLAG  : m->state = LL_I_FLAG_RCV; break;
            default       : m->state = LL_I_START   ; break;
        } break;
    case LL_I_A_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_I_FLAG_RCV; break;
            default       :
                if     (b == ll_get_expected_Iframe_C  ()) m->state = LL_I_C_RCV;
                else if(b == ll_get_unexpected_Iframe_C()) m->state = LL_I_C_UNXP_RCV;
                else if(b == LL_C_SET                    ) m->state = LL_I_C_SET_RCV;
                else                                       m->state = LL_I_START;
                break;
        } break;
    case LL_I_C_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_I_FLAG_RCV; break;
            default       :
                m->state = (
                    b == (LL_A_SEND^ll_get_expected_Iframe_C()) ?
                    LL_I_DATA :
                    LL_I_START
                );
                break;
        } break;
    case LL_I_DATA:
        switch(b){
            case LL_FLAG  : m->state = LL_I_STOP    ; break;
            default       : m->data[m->length++] = b; break;
        } break;
    case LL_I_STOP:
        ll_err("ERROR: can't transition from LL_I_STOP\n");
        return EXIT_FAILURE;
    case LL_I_C_UNXP_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_I_STOP_RR; break;
            default       : m->state = LL_I_C_UNXP_RCV; break;
        }
        break;
    case LL_I_STOP_RR:
        ll_err("ERROR: can't transition from LL_I_STOP_RR\n");
        return EXIT_FAILURE;
    case LL_I_C_SET_RCV:
        switch(b){
            case LL_FLAG  : m->state = LL_I_STOP_UA; break;
            default       : m->state = LL_I_C_SET_RCV; break;
        }
        break;
    case LL_I_STOP_UA:
        ll_err("ERROR: can't transition from LL_I_STOP_UA\n");
        return EXIT_FAILURE;
    default:
        ll_err("ERROR: No such state %d\n", m->state);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

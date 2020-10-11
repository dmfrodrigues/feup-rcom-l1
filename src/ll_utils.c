// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "ll_utils.h"

#include <stdarg.h>
#include <stdio.h>

#include "ll.h"
#include "ll_flags.h"

int ll_log(__attribute__ ((unused)) int verbosity, __attribute__ ((unused)) const char *format, ...){
    #ifdef DEBUG
    if(verbosity <= ll_config.verbosity){
        va_list args;
        va_start(args, format);
        int res = vfprintf(stderr, format, args);
        va_end(args);
        return res;
    }
    #endif
    return 0;
}

int ll_err(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int res = vfprintf(stderr, format, args);
    va_end(args);
    return res;
}

uint8_t ll_bcc(const uint8_t *start, const uint8_t *end){
    uint8_t ret = 0;
    while(start < end){
        ret ^= *(start++);
    }
    return ret;
}

ssize_t ll_stuffing(uint8_t *out, const uint8_t *in, size_t length){
    ssize_t j = 0;
    for(size_t i = 0; i < length; ++i){
        uint8_t c = in[i];
        switch(c){
            case LL_FLAG:
            case LL_ESC :
                out[j++] = LL_ESC;
                out[j++] = LL_STUFF(c);
                break;
            default:
                out[j++] = c;
                break;
        }
    }
    return j;
}

ssize_t ll_destuffing(uint8_t *out, const uint8_t *in, size_t length){
    ssize_t j = 0;
    for(size_t i = 0; i < length; ++i){
        uint8_t c = in[i];
        if(c == LL_ESC){
            uint8_t c_ = in[++i];
            out[j++] = LL_STUFF(c_);
        } else {
            out[j++] = c;
        }
    }
    return j;
}

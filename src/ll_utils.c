#include "ll_utils.h"

#include "ll_flags.h"

uint8_t bcc(const uint8_t *start, const uint8_t *end){
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
            case SP_FLAG:
            case SP_ESC :
                out[j++] = SP_ESC;
                out[j++] = LL_ESCAPE(c);
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
        if(c == SP_ESC){
            uint8_t c_ = in[++i];
            out[j++] = LL_ESCAPE(c_);
        } else {
            out[j++] = c;
        }
    }
    return j;
}

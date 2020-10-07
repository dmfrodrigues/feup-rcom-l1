#include "ll_utils.h"

uint8_t bcc(const uint8_t *start, const uint8_t *end){
    uint8_t ret = 0;
    while(start < end){
        ret ^= *(start++);
    }
    return ret;
}

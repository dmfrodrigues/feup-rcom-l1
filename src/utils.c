#include "utils.h"

uint8_t BCC(uint8_t *start, uint8_t *end){
    uint8_t ret = 0;
    while(start < end){
        ret ^= *(start++);
    }
    return ret;
}

#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

#include <stdint.h>
#include "ll.h"

typedef struct {
    int fileDescriptor; /*Descritor correspondente à porta série*/
    int status; /*TRANSMITTER | RECEIVER*/
} applicationLayer;

#endif // _APPLICATION_LAYER_H_

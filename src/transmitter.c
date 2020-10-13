// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"
#include "application_layer.h"

#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char** argv){

    // CHECK ARGUMENTS
    if(argc < 2){
        printf("Usage:\tnserial SerialPort FileName\n\tex: nserial 2 pinguim.gif\n");
        exit(1);
    }

    if(application(atoi(argv[1]), TRANSMITTER, argv[2]) < 0)
        return 1;

    return 0;
}

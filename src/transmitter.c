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


    /* Just for tests
    // OPEN SERIAL PORT
    int port_fd = llopen(atoi(argv[1]), TRANSMITTER);
    if(port_fd < 0) return -1;

    ssize_t sz;

    char buffer[LL_MAX_SIZE];
    
    strcpy(buffer, "hello");
    sz = llwrite(port_fd, buffer, strlen(buffer));
    if(sz != (ssize_t)strlen(buffer))
        fprintf(stderr, "ERROR: failed to send string '%s'\n", buffer);

    strcpy(buffer, "world");
    sz = llwrite(port_fd, buffer, strlen(buffer));
    if(sz != (ssize_t)strlen(buffer))
        fprintf(stderr, "ERROR: failed to send string '%s'\n", buffer);

    int res = llclose(port_fd);
    if(res) return -1;
    */

    return 0;
}

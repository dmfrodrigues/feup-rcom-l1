#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"

#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char** argv){

    // CHECK ARGUMENTS
    if(argc < 2){
        printf("Usage:\tnserial SerialPort\n\tex: nserial 2\n");
        exit(1);
    }

    // OPEN SERIAL PORT
    int port_fd = llopen(atoi(argv[1]), TRANSMITTER);
    if(port_fd < 0) return -1;

    char buffer[] = "hello";
    ssize_t sz = llwrite(port_fd, buffer, strlen(buffer));
    if(sz != (ssize_t)strlen(buffer)){
        fprintf(stderr, "ERROR: failed to send string '%s'\n", buffer);
    }

    int res = llclose(port_fd);
    if(res) return -1;

    return 0;
}

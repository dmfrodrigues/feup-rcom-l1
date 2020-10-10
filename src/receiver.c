#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int main(int argc, char** argv){
    // CHECK ARGUMENTS
    if(argc < 2){
        printf("Usage:\tnserial SerialPort\n\tex: nserial 2\n");
        exit(1);
    }

    // OPEN SERIAL PORT
    int port_fd = llopen(atoi(argv[1]), RECEIVER);
    if(port_fd < 0) return -1;

    char buffer[LL_MAX_SIZE+1];
    ssize_t sz;
    
    sz = llread(port_fd, buffer);
    if(sz <= 0) fprintf(stderr, "ERROR: failed to receive string\n");
    buffer[sz] = '\0';
    fprintf(stderr, "Read string: '%s'\n", buffer);

    sz = llread(port_fd, buffer);
    if(sz <= 0) fprintf(stderr, "ERROR: failed to receive string\n");
    buffer[sz] = '\0';
    fprintf(stderr, "Read string: '%s'\n", buffer);
    
    int res = llclose(port_fd);
    if(res) return -1;

    return 0;
}

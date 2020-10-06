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

    int res = llclose(port_fd);
    if(res) fprintf(stderr, "Receiver | ERROR");

    return 0;
}

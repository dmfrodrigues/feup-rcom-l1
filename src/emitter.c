/*Non-Canonical Input Processing*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "flags.h"
#include "statemachine.h"
#include "utils.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int timeout=0;

void alarmHandler(){
	fprintf(stderr, "Emitter | WARNING: timeout\n");
	timeout=1;
}

int main(int argc, char** argv){

    // CHECK ARGUMENTS
    if(argc < 2){
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    // OPEN SERIAL PORT
    // O_RDWR   - Open for reading and writing
    // O_NOCTTY - Open serial port not as controlling tty, because we don't want to get killed if linenoise sends CTRL-C
    int port_fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(port_fd < 0){ perror(argv[1]); exit(-1); }

    // SAVE INITIAL PORT SETTINGS
    struct termios oldtio;
    if(tcgetattr(port_fd, &oldtio) == -1) { perror("tcgetattr"); exit(-1); }

    // SET PORT SETTINGS
    // VTIME and VMIN should be changed to protect reads of the following character(s) with a timer
    struct termios newtio;
    bzero(&newtio, sizeof(newtio)); // fills struct newtio with zeros
    newtio.c_cflag      = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag      = IGNPAR;
    newtio.c_oflag      = 0;
    newtio.c_lflag      = 0; // set input mode (non-canonical, no echo,...)
    newtio.c_cc[VTIME]  = 1; // inter-character timer unused
    newtio.c_cc[VMIN]   = 1; // blocking read until 5 chars received

    tcflush(port_fd, TCIOFLUSH);    // flush data received but not read, and data written but not transmitted

    if(tcsetattr(port_fd, TCSANOW, &newtio) == -1) { perror("tcsetattr"); exit(-1); }

    (void) signal(SIGALRM, alarmHandler);
    
    uint8_t SET[5];

    SET[0] = SP_FLAG;
    SET[1] = SP_A_SEND;
    SET[2] = SP_C_SET;
    SET[3] = bcc(SET+1, SET+3);
    SET[4] = SP_FLAG;

    su_state_t state = Start;
    int attempts = 0;
    
    while(attempts < 3 && state != Stop){
        
        attempts++;
        timeout = 0;
        alarm(3);
        
        // Send SET
        int res = write(port_fd, SET, 5);
        if(res != 5) perror("write");
        else         fprintf(stderr, "Emitter | Sent SET\n");

        // Get UA
        do {
            uint8_t byte;
            int res = read(port_fd, &byte, 1);
            if(res > 0){
                fprintf(stderr, "Emitter | Read byte 0x%02X\n", byte);
                state = update_su_state(state, byte);
            }
        } while(state != Stop && !timeout);
        if(timeout) {
            fprintf(stderr, "Emitter | WARNING: gave up due to timeout\n");
            continue;
        }
        // Validation
        if(c_rcv == SP_C_UA && a_rcv == SP_A_SEND) fprintf(stderr, "Emitter | Got UA\n");
        else                                       fprintf(stderr, "Emitter | ERROR: c_rcv or a_rcv are not correct\n");
    }

    // Restore initial port settings
    if(tcsetattr(port_fd, TCSANOW, &oldtio) == -1) { perror("tcsetattr"); exit(-1); }
    // Close port
    close(port_fd);

    return 0;
}

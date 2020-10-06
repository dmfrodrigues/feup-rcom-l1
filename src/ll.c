#include "ll.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "flags.h"
#include "ll_su_statemachine.h"
#include "utils.h"

ll_config_t ll_config = {
    .baud_rate       = 38400,
    .timeout         = 3,
    .retransmissions = 3
};

int timeout = 0;

void alarmHandler(){
	fprintf(stderr, "Emitter | WARNING: timeout\n");
	timeout = 1;
}

tcflag_t ll_get_baud_rate(void){
    if(ll_config.baud_rate < 0     ){ ll_config.baud_rate = 0     ; return B0     ; }
    if(ll_config.baud_rate < 50    ){ ll_config.baud_rate = 50    ; return B50    ; }
    if(ll_config.baud_rate < 75    ){ ll_config.baud_rate = 75    ; return B75    ; }
    if(ll_config.baud_rate < 110   ){ ll_config.baud_rate = 110   ; return B110   ; }
    if(ll_config.baud_rate < 134   ){ ll_config.baud_rate = 134   ; return B134   ; }
    if(ll_config.baud_rate < 150   ){ ll_config.baud_rate = 150   ; return B150   ; }
    if(ll_config.baud_rate < 200   ){ ll_config.baud_rate = 200   ; return B200   ; }
    if(ll_config.baud_rate < 300   ){ ll_config.baud_rate = 300   ; return B300   ; }
    if(ll_config.baud_rate < 600   ){ ll_config.baud_rate = 600   ; return B600   ; }
    if(ll_config.baud_rate < 1200  ){ ll_config.baud_rate = 1200  ; return B1200  ; }
    if(ll_config.baud_rate < 1800  ){ ll_config.baud_rate = 1800  ; return B1800  ; }
    if(ll_config.baud_rate < 2400  ){ ll_config.baud_rate = 2400  ; return B2400  ; }
    if(ll_config.baud_rate < 4800  ){ ll_config.baud_rate = 4800  ; return B4800  ; }
    if(ll_config.baud_rate < 9600  ){ ll_config.baud_rate = 9600  ; return B9600  ; }
    if(ll_config.baud_rate < 19200 ){ ll_config.baud_rate = 19200 ; return B19200 ; }
    if(ll_config.baud_rate < 38400 ){ ll_config.baud_rate = 38400 ; return B38400 ; }
    if(ll_config.baud_rate < 57600 ){ ll_config.baud_rate = 57600 ; return B57600 ; }
    if(ll_config.baud_rate < 115200){ ll_config.baud_rate = 115200; return B115200; }
    ll_config.baud_rate = 230400; return B230400;
}

int ll_send_SET(int port_fd){
    uint8_t SET[5];
    SET[0] = SP_FLAG;
    SET[1] = SP_A_SEND;
    SET[2] = SP_C_SET;
    SET[3] = bcc(SET+1, SET+3);
    SET[4] = SP_FLAG;

    int res = write(port_fd, SET, sizeof(SET));
    if(res == sizeof(SET)) fprintf(stderr, "Sent SET\n");
    return res;
}

int ll_send_UA(int port_fd){
    uint8_t UA[5];
    UA[0] = SP_FLAG;
    UA[1] = SP_A_SEND;
    UA[2] = SP_C_UA;
    UA[3] = bcc(UA+1, UA+3);
    UA[4] = SP_FLAG;

    int res = write(port_fd, UA, sizeof(UA));
    if(res == sizeof(UA))  fprintf(stderr, "Sent UA\n");
    return res;
}

int ll_expect_SUframe(int port_fd, uint8_t *a_rcv, uint8_t *c_rcv){
    ll_su_statemachine_t machine = {
        .state = Start,
        .a_rcv = 0,
        .c_rcv = 0
    };
    do {
        uint8_t byte;
        int res = read(port_fd, &byte, 1);
        if(res == 1){
            fprintf(stderr, "Read byte 0x%02X\n", byte);
            fprintf(stderr, "Transitioned from state %d", machine.state);
            res = ll_su_state_update(&machine, byte);
            fprintf(stderr, " to %d\n", machine.state);
            if(res) fprintf(stderr, "ERROR: failed to update state\n");
        } else {
            perror("read");
            return EXIT_FAILURE;
        }
    } while(machine.state != Stop);
    *a_rcv = machine.a_rcv;
    *c_rcv = machine.c_rcv;
    return EXIT_SUCCESS;
}

struct termios oldtio;

int llopen(int com, ll_status_t status){
    // Get serial port path
    char port_path[255];
    sprintf(port_path, "/dev/ttyS%d", com-1);

    // Open serial port
    // O_RDWR   - Open for reading and writing
    // O_NOCTTY - Open serial port not as controlling tty, because we don't want to get killed if linenoise sends CTRL-C
    int port_fd = open(port_path, O_RDWR | O_NOCTTY);
    if(port_fd < 0){ perror(port_path); exit(-1); }

    // Save initial port settings
    if(tcgetattr(port_fd, &oldtio) == -1) { perror("tcgetattr"); exit(-1); }

    // Set port settings
    // VTIME and VMIN should be changed to protect reads of the following character(s) with a timer
    struct termios newtio;
    bzero(&newtio, sizeof(newtio)); // fills struct newtio with zeros
    tcflag_t baud_rate = ll_get_baud_rate();
    newtio.c_cflag      = baud_rate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag      = IGNPAR;
    newtio.c_oflag      = 0;
    newtio.c_lflag      = 0; // set input mode (non-canonical, no echo,...)
    newtio.c_cc[VTIME]  = 1; // inter-character timer unused
    newtio.c_cc[VMIN]   = 1; // blocking read until 5 chars received

    tcflush(port_fd, TCIOFLUSH);    // flush data received but not read, and data written but not transmitted

    if(tcsetattr(port_fd, TCSANOW, &newtio) == -1) { perror("tcsetattr"); exit(-1); }

    int res;

    if(status == TRANSMITTER){
        struct sigaction action;
        action.sa_handler = alarmHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGALRM, &action, NULL);

        int attempts;
        for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
            timeout = 0;
            alarm(ll_config.timeout);
            
            // Send SET
            res = ll_send_SET(port_fd);

            // Get UA
            uint8_t a_rcv, c_rcv;
            res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);
            
            // Validate UA
            if(res){
                if(errno == EINTR){
                    if(timeout) fprintf(stderr, "WARNING: gave up due to timeout\n");
                    else        fprintf(stderr, "ERROR: emitter was interrupted due to unknown reason\n");
                } else perror("read");
            } else if(c_rcv == SP_C_UA && a_rcv == SP_A_SEND){
                fprintf(stderr, "Got UA\n");
                break;
            } else fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\n");
        }
        if(attempts == ll_config.retransmissions) return EXIT_FAILURE;
        alarm(0);
    } else if(status == RECEIVER){
        // Get SET
        uint8_t a_rcv, c_rcv;
        res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);
        if(res){
            perror("read");
        } else if(a_rcv == SP_C_SET && c_rcv == SP_A_SEND){
            fprintf(stderr, "Got SET\n");
            res = ll_send_UA(port_fd);
        } else {
            fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\n");
        }
    }

    return port_fd;
}

int llclose(int port_fd){
    // Restore initial port settings
    if(tcsetattr(port_fd, TCSANOW, &oldtio) == -1) { perror("tcsetattr"); exit(-1); }
    // Close port
    close(port_fd);

    return EXIT_SUCCESS;
}

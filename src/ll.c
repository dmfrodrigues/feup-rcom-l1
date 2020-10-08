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
#include <stdbool.h>

#include "ll_flags.h"
#include "ll_su_statemachine.h"
#include "ll_utils.h"

ll_config_t ll_config = {
    .baud_rate       = 38400,
    .timeout         = 3,
    .retransmissions = 3
};

ll_status_t ll_status;
int timeout = 0;
unsigned int sequence_number = 1;

/**
 * @brief Handle alarm signal.
 * 
 * @param signum    Signal number (unused)
 */
void alarmHandler(__attribute__((unused)) int signum);

/**
 * @brief Get baud rate.
 * 
 * The baud rate can be specified as any integer in ll_config.baud_rate.
 * However, serial port drivers only support certain baud rates,
 * so this function takes the specified baud rate, rounds it up to the
 * next valid baud rate, and returns the corresponding baud rate flag
 * to use in termios.c_cflag.
 * 
 * @return tcflag_t     Valid baud rate flag for termios.c_cflag
 */
tcflag_t ll_get_baud_rate(void) __attribute__((warn_unused_result));

/**
 * @brief Get RR byte expected from the other end of the communication.
 * 
 * @return uint8_t  Expected RR byte
 */
uint8_t ll_get_expected_RR(void) __attribute__((warn_unused_result));

/**
 * @brief Get REJ byte expected from the other end of the communication.
 * 
 * @return uint8_t  Expected REJ byte
 */
uint8_t ll_get_expected_REJ(void) __attribute__((warn_unused_result));

/**
 * @brief Get I-frame C byte (according to the value of sequence_number, aka Ns).
 * 
 * @return uint8_t  I-frame C byte
 */
uint8_t ll_get_Iframe_C(void) __attribute__((warn_unused_result));

/**
 * @brief Send SET message (a S-frame) to serial port.
 * 
 * @param port_fd   Port to send SET message to
 * @return int      On success, the number of bytes written; on error, -1, and errno is set
 */
int ll_send_SET(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send DISC message (a S-frame) to serial port.
 * 
 * @param port_fd   Port to send DISC message to
 * @return int      On success, the number of bytes written; on error, -1, and errno is set
 */
int ll_send_DISC(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send UA message (a U-frame) to serial port.
 * 
 * @param port_fd   Port to send UA message to
 * @return int      On success, the number of bytes written; on error, -1, and errno is set
 */
int ll_send_UA(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send data message (I-frame) to serial port.
 * 
 * @param port_fd   Port to send data message to
 * @param buffer    Raw (destuffed) data to send to port
 * @param length    Length of data to send, in bytes
 * @return ssize_t  On success, the number of bytes written; on error, -1, and errno is set
 */
ssize_t ll_send_I(int port_fd, const uint8_t *buffer, size_t length) __attribute__((warn_unused_result));

/**
 * @brief Send Receiver Ready (RR) to port.
 * 
 * @param port_fd   Port to send RR to
 * @return int      0 if successful; -1 otherwise
 */
int ll_send_RR(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Send Rejected (REJ) to port.
 * 
 * @param port_fd   Port to send REJ to
 * @return int      Number of bytes sent, if successful; -1 otherwise
 */
int ll_send_REJ(int port_fd) __attribute__((warn_unused_result));

/**
 * @brief Expect for S/U-frame to arrive from port.
 * 
 * @param port_fd   Port to expect the frame comes from
 * @param a_rcv     Pointer to memory where address (A) byte will be saved
 * @param c_rcv     Pointer to memory where control (C) byte will be saved
 * @return int      On success, 0; on error, another value, and errno is set
 */
int ll_expect_SUframe(int port_fd, uint8_t *a_rcv, uint8_t *c_rcv) __attribute__((warn_unused_result));

void alarmHandler(__attribute__((unused)) int signum){
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

uint8_t ll_get_expected_RR(void){
    uint8_t Ns = sequence_number;
    uint8_t Nr = (Ns+1)%2;
    return LL_RR(Nr);
}

uint8_t ll_get_expected_REJ(void){
    uint8_t Ns = sequence_number;
    uint8_t Nr = (Ns+1)%2;
    return LL_REJ(Nr);
}

uint8_t ll_get_Iframe_C(void){
    uint8_t Ns = sequence_number;
    return LL_C(Ns);
}

int ll_send_SET(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_SEND : LL_A_RECV);;
    frame[2] = LL_C_SET;
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame));
    if(res == sizeof(frame)) fprintf(stderr, "Sent SET\n");
    return res;
}

int ll_send_DISC(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_SEND : LL_A_RECV);
    frame[2] = LL_C_DISC;
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame));
    if(res == sizeof(frame))  fprintf(stderr, "Sent DISC\n");
    return res;
}

int ll_send_UA(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame[2] = LL_C_UA;
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame));
    if(res == sizeof(frame))  fprintf(stderr, "Sent UA\n");
    return res;
}

ssize_t ll_send_I(int port_fd, const uint8_t *buffer, size_t length){
    uint8_t frame_header[4];
    frame_header[0] = LL_FLAG;
    frame_header[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame_header[2] = ll_get_Iframe_C();
    frame_header[3] = ll_bcc(frame_header+1, frame_header+3);
    if(write(port_fd, frame_header, sizeof(frame_header)) != sizeof(frame_header)){ perror("write"); return -1; }
    
    uint8_t buffer_escaped[2*LL_MAX_SIZE];
    size_t written_chars = ll_stuffing(buffer_escaped, buffer, length);
    if(written_chars < length) return -1;

    uint8_t frame_tail[2];
    frame_tail[0] = ll_bcc(buffer, buffer+length);
    frame_tail[1] = LL_FLAG;
    if(write(port_fd, frame_tail, sizeof(frame_tail)) != sizeof(frame_tail)){ perror("write"); return -1; }

    return length;
}

int ll_send_RR(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame[2] = LL_RR(sequence_number);
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame));
    if(res == sizeof(frame)){
        fprintf(stderr, "Sent RR\n");
        return EXIT_SUCCESS;
    } else return EXIT_FAILURE;
}

int ll_send_REJ(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame[2] = LL_REJ(sequence_number);
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame));
    if(res == sizeof(frame)){
        fprintf(stderr, "Sent REJ\n");
        return EXIT_SUCCESS;
    } else return EXIT_FAILURE;
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
    ll_status = status;

    // Get serial port path
    char port_path[255];
    sprintf(port_path, "/dev/ttyS%d", com-1);

    // Open serial port
    // O_RDWR   - Open for reading and writing
    // O_NOCTTY - Open serial port not as controlling tty, because we don't want to get killed if linenoise sends CTRL-C
    int port_fd = open(port_path, O_RDWR | O_NOCTTY);
    if(port_fd < 0){ perror(port_path); return -1; }

    // Save initial port settings
    if(tcgetattr(port_fd, &oldtio) == -1) { perror("tcgetattr"); return -1; }

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

    if(tcsetattr(port_fd, TCSANOW, &newtio) == -1) { perror("tcsetattr"); return -1; }

    int res;

    if(status == TRANSMITTER){
        struct sigaction action;
        action.sa_handler = alarmHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGALRM, &action, NULL);

        unsigned attempts;
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
            } else if(a_rcv == LL_A_SEND && c_rcv == LL_C_UA){
                fprintf(stderr, "Got UA\n");
                break;
            } else{
                fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\na_rcv=0x%02X (should be 0x%02X)\nc_rcv=0x%02X (should be 0x%02X)\n", a_rcv, LL_A_SEND, c_rcv, LL_C_UA);
            }
        }
        if(attempts == ll_config.retransmissions) return -1;
        alarm(0);
    } else if(status == RECEIVER){
        // Get SET
        uint8_t a_rcv, c_rcv;
        res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);
        if(res){
            perror("read");
            return -1;
        } else if(a_rcv == LL_A_SEND && c_rcv == LL_C_SET){
            fprintf(stderr, "Got SET\n");
            res = ll_send_UA(port_fd);
        } else {
            fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\na_rcv=0x%02X (should be 0x%02X)\nc_rcv=0x%02X (should be 0x%02X)\n", a_rcv, LL_A_SEND, c_rcv, LL_C_SET);
            return -1;
        }
    }

    return port_fd;
}

int llwrite(int port_fd, const char *buffer, int length){
    sequence_number = (sequence_number+1)%2;

    int res, ret = 0;
    unsigned attempts;
    for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
        timeout = 0;
        alarm(ll_config.timeout);
        
        // Send SET
        ret = ll_send_I(port_fd, (const uint8_t *)buffer, length);
        if(ret != length){
            fprintf(stderr, "ERROR: only wrote %d chars\n", ret);
            continue;
        }

        // Get UA
        uint8_t a_rcv, c_rcv;
        res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);
        
        // Validate UA
        if(res){
            if(errno == EINTR){
                if(timeout) fprintf(stderr, "WARNING: gave up due to timeout\n");
                else        fprintf(stderr, "ERROR: emitter was interrupted due to unknown reason\n");
            } else perror("read");
        } else if(a_rcv == LL_A_SEND){
            if(c_rcv == ll_get_expected_RR()){
                fprintf(stderr, "Got RR\n");
                break;
            } else if(c_rcv == ll_get_expected_REJ()){
                fprintf(stderr, "Got REJ\n");
            } else fprintf(stderr, "Don't know what I got; a=0x%02X, c=0x%02X\n", a_rcv, c_rcv);
        } else{
            fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\na_rcv=0x%02X (should be 0x%02X)\nc_rcv=0x%02X (should be 0x%02X, 0x%02X)\n", a_rcv, LL_A_SEND, c_rcv, ll_get_expected_RR(), ll_get_expected_REJ());
        }
    }
    if(attempts == ll_config.retransmissions) return -1;
    alarm(0);

    return ret;
}

int llread(int port_fd, const char *buffer){
    //TODO
    return -1;
}

int llclose(int port_fd){
    int res;

    if(ll_status == TRANSMITTER){  
        unsigned attempts;
        for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
            timeout = 0;
            alarm(ll_config.timeout);
            
            // Send DISC
            res = ll_send_DISC(port_fd);

            // Get DISC
            uint8_t a_rcv, c_rcv;
            res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);
            
            // Validate DISC
            if(res){
                if(errno == EINTR){
                    if(timeout) fprintf(stderr, "WARNING: gave up due to timeout\n");
                    else        fprintf(stderr, "ERROR: interrupted due to unknown reason\n");
                } else perror("read");
            } 
            else if(a_rcv == LL_A_RECV && c_rcv == LL_C_DISC) {
                fprintf(stderr, "Got DISC\n");
                break;
            } else fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\n");
        }
    
        if(attempts == ll_config.retransmissions) return -1;

        // Send UA
        res = ll_send_UA(port_fd);
        alarm(0);
    }
    else{
        // Get DISC
        uint8_t a_rcv, c_rcv;
        res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);

        if(res){
            perror("read"); return -1;
        }
        else if(a_rcv == LL_A_SEND && c_rcv == LL_C_DISC){
            fprintf(stderr, "Got DISC\n");
        } else{ 
            fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\n"); return -1;
        }
        // Send DISC
        res = ll_send_DISC(port_fd);


        // Get UA
        res = ll_expect_SUframe(port_fd, &a_rcv, &c_rcv);

        if(res){
            perror("read"); return -1;
        }
        else if(a_rcv == LL_A_RECV && c_rcv == LL_C_UA){
            fprintf(stderr, "Got UA\n");
        } else{ 
            fprintf(stderr, "ERROR: c_rcv or a_rcv are not correct\n"); return -1;
        }
    }

    // Restore initial port settings
    sleep(1);
    if(tcsetattr(port_fd, TCSANOW, &oldtio) == -1) { perror("tcsetattr"); return -1; }
    // Close port
    if(close(port_fd) == -1) { perror("close"); return -1; };
    
    fprintf(stderr, "Successfully disconnected\n");

    return 0;
}

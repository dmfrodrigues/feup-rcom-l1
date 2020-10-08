#include "ll_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ll_flags.h"
#include "ll_i_statemachine.h"
#include "ll_su_statemachine.h"
#include "ll_utils.h"

ll_config_t ll_config = {
    .baud_rate       = 38400,
    .timeout         = 3,
    .retransmissions = 3
};
int timeout = 0;
unsigned int sequence_number = 1;

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

uint8_t ll_get_expected_Iframe_C(void){
    uint8_t Ns = sequence_number;
    uint8_t Nr = (Ns+1)%2;
    return LL_C(Nr);
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
    ssize_t written_chars = ll_stuffing(buffer_escaped, buffer, length);
    if(written_chars < (ssize_t)length) return -1;

    uint8_t bcc2 = ll_bcc(buffer, buffer+length);
    if(bcc2 == LL_FLAG || bcc2 == LL_ESC){
        uint8_t frame_tail[3];
        frame_tail[0] = LL_ESC;
        frame_tail[1] = LL_STUFF(bcc2);
        frame_tail[2] = LL_FLAG;
        if(write(port_fd, frame_tail, sizeof(frame_tail)) != sizeof(frame_tail)){ perror("write"); return -1; }
    } else {
        uint8_t frame_tail[2];
        frame_tail[0] = bcc2;
        frame_tail[1] = LL_FLAG;
        if(write(port_fd, frame_tail, sizeof(frame_tail)) != sizeof(frame_tail)){ perror("write"); return -1; }
    }
    
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
        .state = LL_SU_Start,
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
    } while(machine.state != LL_SU_Stop);
    *a_rcv = machine.a_rcv;
    *c_rcv = machine.c_rcv;
    return EXIT_SUCCESS;
}

ssize_t ll_expect_Iframe(int port_fd, uint8_t *buffer){
    ll_i_statemachine_t machine = {
        .state = LL_I_Start,
        .escaped = false,
        .length = 0
    };
    do {
        uint8_t byte;
        int res = read(port_fd, &byte, 1);
        if(res == 1){
            fprintf(stderr, "Read byte 0x%02X\n", byte);
            fprintf(stderr, "Transitioned from state %d", machine.state);
            res = ll_i_state_update(&machine, byte);
            fprintf(stderr, " to %d\n", machine.state);
            if(res){
                fprintf(stderr, "ERROR: failed to update state\n");
                return -1;
            }
        } else {
            perror("read");
            return -1;
        }
    } while(machine.state != LL_I_Stop);

    ssize_t written_chars = ll_destuffing(buffer, machine.data, machine.length);
    if(written_chars <= 0) return -1;

    uint8_t bcc2  = buffer[written_chars];
    uint8_t bcc2_ = ll_bcc(buffer, buffer+written_chars-1);
    if(bcc2 != bcc2_){
        fprintf(stderr, "ERROR: bcc2 is not correct (is 0x%02X, should be 0x%02X)\n", bcc2, bcc2_);
        return -1;
    }

    return written_chars-1;
}

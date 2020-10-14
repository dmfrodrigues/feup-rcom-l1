// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "ll_internal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "stats.h"

ll_config_t ll_config = {
    .baud_rate       = 38400,
    .timeout         = 3,
    .retransmissions = 3,
    .verbosity       = 3
};
int timeout = 0;
unsigned int sequence_number;

void alarmHandler(__attribute__((unused)) int signum){
	ll_log(1, "WARNING: timeout\n");
	timeout = 1;
}

tcflag_t ll_get_baud_rate(void){
    if(ll_config.baud_rate <= 0     ){ ll_config.baud_rate = 0     ; return B0     ; }
    if(ll_config.baud_rate <= 50    ){ ll_config.baud_rate = 50    ; return B50    ; }
    if(ll_config.baud_rate <= 75    ){ ll_config.baud_rate = 75    ; return B75    ; }
    if(ll_config.baud_rate <= 110   ){ ll_config.baud_rate = 110   ; return B110   ; }
    if(ll_config.baud_rate <= 134   ){ ll_config.baud_rate = 134   ; return B134   ; }
    if(ll_config.baud_rate <= 150   ){ ll_config.baud_rate = 150   ; return B150   ; }
    if(ll_config.baud_rate <= 200   ){ ll_config.baud_rate = 200   ; return B200   ; }
    if(ll_config.baud_rate <= 300   ){ ll_config.baud_rate = 300   ; return B300   ; }
    if(ll_config.baud_rate <= 600   ){ ll_config.baud_rate = 600   ; return B600   ; }
    if(ll_config.baud_rate <= 1200  ){ ll_config.baud_rate = 1200  ; return B1200  ; }
    if(ll_config.baud_rate <= 1800  ){ ll_config.baud_rate = 1800  ; return B1800  ; }
    if(ll_config.baud_rate <= 2400  ){ ll_config.baud_rate = 2400  ; return B2400  ; }
    if(ll_config.baud_rate <= 4800  ){ ll_config.baud_rate = 4800  ; return B4800  ; }
    if(ll_config.baud_rate <= 9600  ){ ll_config.baud_rate = 9600  ; return B9600  ; }
    if(ll_config.baud_rate <= 19200 ){ ll_config.baud_rate = 19200 ; return B19200 ; }
    if(ll_config.baud_rate <= 38400 ){ ll_config.baud_rate = 38400 ; return B38400 ; }
    if(ll_config.baud_rate <= 57600 ){ ll_config.baud_rate = 57600 ; return B57600 ; }
    if(ll_config.baud_rate <= 115200){ ll_config.baud_rate = 115200; return B115200; }
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

uint8_t ll_get_unexpected_Iframe_C(void){
    uint8_t Ns = (sequence_number+1)%2;
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

    int res = write(port_fd, frame, sizeof(frame)); ADD_MESSAGE_LENGTH(sizeof(frame)); ADD_FRAME();
    if(res == sizeof(frame)) ll_log(2, "    Sent SET\n");
    return res;
}

int ll_send_DISC(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_SEND : LL_A_RECV);
    frame[2] = LL_C_DISC;
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame)); ADD_MESSAGE_LENGTH(sizeof(frame)); ADD_FRAME();
    if(res == sizeof(frame))  ll_log(2, "    Sent DISC\n");
    return res;
}

int ll_send_UA(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame[2] = LL_C_UA;
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame)); ADD_MESSAGE_LENGTH(sizeof(frame)); ADD_FRAME();
    if(res == sizeof(frame))  ll_log(2, "    Sent UA\n");
    return res;
}

ssize_t ll_send_I(int port_fd, const uint8_t *buffer, size_t length){
    ll_log(2, "    Sending I-frame\n");
    
    for(size_t i = 0; i < length; ++i) ll_log(3, "        Sending byte %02X\n", buffer[i]);

    uint8_t frame_header[4];
    frame_header[0] = LL_FLAG;
    frame_header[1] = (ll_status == TRANSMITTER ? LL_A_SEND : LL_A_RECV);
    frame_header[2] = ll_get_Iframe_C();
    frame_header[3] = ll_bcc(frame_header+1, frame_header+3);

    GEN_FRAME_ERROR(2e-5, frame_header, sizeof(frame_header));

    if(write(port_fd, frame_header, sizeof(frame_header)) != sizeof(frame_header))
        { perror("write"); return -1; } ADD_MESSAGE_LENGTH(sizeof(frame_header));
    
    uint8_t buffer_escaped[2*LL_MAX_SIZE];
    ssize_t written_chars = ll_stuffing(buffer_escaped, buffer, length);
    if(written_chars < (ssize_t)length) return -1;
    ll_log(2, "    Stuffed bits, preparing to send %ld bytes", written_chars);
    for(ssize_t i = 0; i < written_chars; ++i){
        ll_log(3, " %02X", buffer_escaped[i]);
    }
    ll_log(2, "\n");

    GEN_FRAME_ERROR(1e-5, buffer_escaped, written_chars);

    if(write(port_fd, buffer_escaped, written_chars) != written_chars)
        { perror("write"); return -1; } ADD_MESSAGE_LENGTH(written_chars);

    uint8_t bcc2 = ll_bcc(buffer, buffer+length);
    if(bcc2 == LL_FLAG || bcc2 == LL_ESC){
        uint8_t frame_tail[3];
        frame_tail[0] = LL_ESC;
        frame_tail[1] = LL_STUFF(bcc2);
        frame_tail[2] = LL_FLAG;

        GEN_FRAME_ERROR(2e-5, frame_tail, sizeof(frame_tail));

        if(write(port_fd, frame_tail, sizeof(frame_tail)) != sizeof(frame_tail))
            { perror("write"); return -1; } ADD_MESSAGE_LENGTH(sizeof(frame_tail));
    } else {
        uint8_t frame_tail[2];
        frame_tail[0] = bcc2;
        frame_tail[1] = LL_FLAG;

        GEN_FRAME_ERROR(2e-5, frame_tail, sizeof(frame_tail));

        if(write(port_fd, frame_tail, sizeof(frame_tail)) != sizeof(frame_tail))
            { perror("write"); return -1; } ADD_MESSAGE_LENGTH(sizeof(frame_tail));
    }

    ADD_FRAME();

    ll_log(2, "    Sent I\n");
    
    return length;
}

int ll_send_RR(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame[2] = LL_RR(sequence_number);
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame)); ADD_MESSAGE_LENGTH(sizeof(frame)); ADD_FRAME();
    if(res == sizeof(frame)){
        ll_log(2, "    Sent RR\n");
        return EXIT_SUCCESS;
    } else return EXIT_FAILURE;
}

int ll_send_RR_resend(int port_fd){
    uint8_t frame[5];
    frame[0] = LL_FLAG;
    frame[1] = (ll_status == TRANSMITTER ? LL_A_RECV : LL_A_SEND);
    frame[2] = LL_RR((sequence_number+1)%2);
    frame[3] = ll_bcc(frame+1, frame+3);
    frame[4] = LL_FLAG;

    int res = write(port_fd, frame, sizeof(frame)); ADD_MESSAGE_LENGTH(sizeof(frame)); ADD_FRAME();
    if(res == sizeof(frame)){
        ll_log(2, "    Sent RR resend\n");
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

    int res = write(port_fd, frame, sizeof(frame)); ADD_MESSAGE_LENGTH(sizeof(frame)); ADD_FRAME();
    if(res == sizeof(frame)){
        ll_log(2, "    Sent REJ\n");
        return EXIT_SUCCESS;
    } else return EXIT_FAILURE;
}

int ll_expect_Sframe(int port_fd, uint8_t *a_rcv, uint8_t *c_rcv){
    ll_log(2, "    Expecting S-frame\n");
    ll_s_statemachine_t machine;
    do {
        machine.state = LL_S_START;
        machine.a_rcv = 0;
        machine.c_rcv = 0;
        do {
            uint8_t byte;
            ADD_DELAY(1000);
            int res = read(port_fd, &byte, 1);
            if(res == 1){
                ll_log(3, "        Read byte 0x%02X | ", byte);
                ll_log(3, "Transitioned from state %d", machine.state);
                res = ll_s_state_update(&machine, byte);
                ll_log(3, " to %d\n", machine.state);
                if(res) ll_err("ERROR: failed to update state\n");
            } else {
                perror("read");
                return EXIT_FAILURE;
            }
        } while(machine.state != LL_S_STOP && machine.state != LL_S_STOP_RR);
        if(machine.state == LL_S_STOP_RR){
            ADD_FRAME_ERROR();
            ll_log(2, "    Got unexpected I-frame (sending RR)\n");
            if(ll_send_RR_resend(port_fd)){
                ll_err("ERROR: Failed to send RR\n");
                return -1;
            }
        }
    } while(machine.state != LL_S_STOP);
    *a_rcv = machine.a_rcv;
    *c_rcv = machine.c_rcv;
    return EXIT_SUCCESS;
}

int ll_expect_Uframe(int port_fd, uint8_t *a_rcv, uint8_t *c_rcv){
    ll_log(2, "    Expecting U-frame\n");
    ll_u_statemachine_t machine;
    do {
        machine.state = LL_U_START;
        machine.a_rcv = 0;
        machine.c_rcv = 0;
        do {
            uint8_t byte;
            ADD_DELAY(1000);
            int res = read(port_fd, &byte, 1);
            if(res == 1){
                ll_log(3, "        Read byte 0x%02X | ", byte);
                ll_log(3, "Transitioned from state %d", machine.state);
                res = ll_u_state_update(&machine, byte);
                ll_log(3, " to %d\n", machine.state);
                if(res) ll_err("ERROR: failed to update state\n");
            } else {
                perror("read");
                return EXIT_FAILURE;
            }
        } while(machine.state != LL_U_STOP && machine.state != LL_U_STOP_DISC);
        if(machine.state == LL_U_STOP_DISC){
            ADD_FRAME_ERROR();
            ll_log(2, "    Got unexpected DISC (sending DISC as well)\n");
            if(ll_send_DISC(port_fd)){
                ll_err("ERROR: Failed to send DISC\n");
                return -1;
            }
        }
    } while(machine.state != LL_U_STOP);
    *a_rcv = machine.a_rcv;
    *c_rcv = machine.c_rcv;
    return EXIT_SUCCESS;
}

ssize_t ll_expect_Iframe(int port_fd, uint8_t *buffer){
    ll_log(2, "    Expecting I-frame\n");
    ll_i_statemachine_t machine;
    do{
        machine.state = LL_I_START;
        machine.length = 0;
        do {
            uint8_t byte;
            ADD_DELAY(1000);
            int res = read(port_fd, &byte, 1);
            if(res == 1){
                ll_log(3, "        Read byte 0x%02X | ", byte);
                ll_log(3, "Transitioned from state %d", machine.state);
                res = ll_i_state_update(&machine, byte);
                ll_log(3, " to %d\n", machine.state);
                if(res){
                    ll_err("ERROR: failed to update state\n");
                    return -1;
                }
            } else {
                perror("read");
                return -1;
            }
        } while(machine.state != LL_I_STOP && machine.state != LL_I_STOP_RR);
        if(machine.state == LL_I_STOP_RR){
            ADD_FRAME_ERROR();
            ll_log(2, "    Got unexpected data (sending RR and ignoring data)\n");
            if(ll_send_RR_resend(port_fd)){
                ll_err("ERROR: Failed to send RR\n");
                return -1;
            }
        }
    } while(machine.state != LL_I_STOP);

    ssize_t written_chars = ll_destuffing(buffer, machine.data, machine.length);
    if(written_chars <= 0) return -1;

    uint8_t bcc2  = buffer[written_chars-1];
    uint8_t bcc2_ = ll_bcc(buffer, buffer+written_chars-1);
    if(bcc2 != bcc2_){
        ll_err("ERROR: bcc2 incorrect (is 0x%02X, should be 0x%02X)\n", bcc2, bcc2_);
        return -1;
    }

    return written_chars-1;
}

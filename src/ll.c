// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "ll.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include "ll_flags.h"
#include "ll_internal.h"
#include "ll_utils.h"

#include "stats.h"

struct termios oldtio;

struct itimerval ll_timer_reset = {
    {0, 0},
    {0, 0}
};

int llopen(int com, ll_status_t status){
    ll_log(1, "Preparing to open\n");

    ll_status = status;

    // Get serial port path
    char port_path[255];
    sprintf(port_path, "/dev/ttyS%d", com-1);

    // Open serial port
    // O_RDWR   - Open for reading and writing
    // O_NOCTTY - Open serial port not as controlling tty,
    //            because we don't want to get killed if linenoise sends CTRL-C
    int port_fd = open(port_path, O_RDWR | O_NOCTTY);
    if(port_fd < 0){ perror(port_path); return -1; }

    // Save initial port settings
    if(tcgetattr(port_fd, &oldtio) == -1) { perror("tcgetattr"); return -1; }

    // Set port settings
    // VTIME/VMIN should be changed to protect reads of following character(s) with timer
    struct termios newtio;
    bzero(&newtio, sizeof(newtio)); // fills struct newtio with zeros
    tcflag_t baud_rate = ll_get_baud_rate();
    newtio.c_cflag      = baud_rate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag      = IGNPAR;
    newtio.c_oflag      = 0;
    newtio.c_lflag      = 0; // set input mode (non-canonical, no echo,...)
    newtio.c_cc[VTIME]  = 1; // inter-character timer unused
    newtio.c_cc[VMIN]   = 1; // blocking read until 5 chars received

    // flush data received but not read, and data written but not transmitted
    tcflush(port_fd, TCIOFLUSH);

    if(tcsetattr(port_fd, TCSANOW, &newtio) == -1) { perror("tcsetattr"); return -1; }

    int res;

    switch(status){
        case TRANSMITTER: sequence_number = 1; break;
        case RECEIVER   : sequence_number = 0; break;
        default: ll_err("ERROR: no such status %d\n", status); return -1;
    }

    if(status == TRANSMITTER){
        struct sigaction action;
        action.sa_handler = alarmHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGALRM, &action, NULL);

        unsigned attempts;
        for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
            timeout = 0;
            setitimer(ITIMER_REAL, &ll_config.timeout, NULL);
            
            // Send SET
            res = ll_send_SET(port_fd);

            // Get UA
            uint8_t a_rcv, c_rcv;
            res = ll_expect_Uframe(port_fd, &a_rcv, &c_rcv);
            
            // Validate UA
            if(res){
                if(errno == EINTR){
                    if(timeout){
                        ll_err("WARNING: timeout waiting for UA\n");
                        ADD_FRAME_TIMEOUT();
                    } else        ll_err("ERROR: emitter was interrupted\n");
                } else perror("read");
            } else if(a_rcv == LL_A_SEND && c_rcv == LL_C_UA){
                ll_log(2, "    Got UA\n");
                break;
            } else{
                ll_err("ERROR: c_rcv or a_rcv are not correct\n");
                ll_err("a_rcv=0x%02X (should be 0x%02X)\n", a_rcv, LL_A_SEND);
                ll_err("c_rcv=0x%02X (should be 0x%02X)\n", c_rcv, LL_C_UA);
            }
        }
        if(attempts == ll_config.retransmissions){
            if(timeout) ll_err("WARNING: gave up due to timeout\n");
            return -1;
        }
        setitimer(ITIMER_REAL, &ll_timer_reset, NULL);
    } else if(status == RECEIVER){
        struct sigaction action;
        action.sa_handler = alarmHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGALRM, &action, NULL);

        unsigned attempts;
            for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
            timeout = 0;
            setitimer(ITIMER_REAL, &ll_config.timeout, NULL);

            // Get SET
            uint8_t a_rcv, c_rcv;
            res = ll_expect_Sframe(port_fd, &a_rcv, &c_rcv);
            if(res){
                if(errno == EINTR){
                    if(timeout){
                        ll_err("WARNING: timeout waiting for SET\n");
                        ADD_FRAME_TIMEOUT();
                    } else        ll_err("ERROR: receiver was interrupted\n");
                } else perror("read");
            } else if(a_rcv == LL_A_SEND && c_rcv == LL_C_SET){
                ll_log(2, "    Got SET\n");
                res = ll_send_UA(port_fd);
                if(res < 0) return -1;
                break;
            } else {
                ll_err("ERROR: c_rcv or a_rcv are not correct\n");
                ll_err("a_rcv=0x%02X (should be 0x%02X)\n", a_rcv, LL_A_SEND);
                ll_err("c_rcv=0x%02X (should be 0x%02X)\n", c_rcv, LL_C_SET);
                return -1;
            }
        }
        if(attempts == ll_config.retransmissions){
            if(timeout) ll_err("WARNING: gave up due to timeout\n");
            return -1;
        }
        setitimer(ITIMER_REAL, &ll_timer_reset, NULL);
    }

    ll_log(1, "Successfully opened\n");

    return port_fd;
}

int llwrite(int port_fd, const uint8_t *buffer, int length){
    if(length > LL_MAX_SIZE){
        ll_err("ERROR: length=%d larger than LL_MAX_SIZE=%d\n", length, LL_MAX_SIZE);
        return -1;
    }

    ll_log(1, "Preparing to write\n");

    if(ll_status == RECEIVER) return -1;

    sequence_number = (sequence_number+1)%2;

    int res, ret = 0;
    unsigned attempts;
    for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
        timeout = 0;
        setitimer(ITIMER_REAL, &ll_config.timeout, NULL);
        
        // Send I
        ret = ll_send_I(port_fd, (const uint8_t *)buffer, length);
        if(ret != length){
            ll_err("ERROR: only wrote %d chars\n", ret);
            continue;
        }

        // Get RR or REJ
        uint8_t a_rcv, c_rcv;
        res = ll_expect_Sframe(port_fd, &a_rcv, &c_rcv);
        
        // Validate RR or REJ
        if(res){
            if(errno == EINTR){
                if(timeout){
                    ll_err("WARNING: timeout waiting for RR/REJ\n");
                    ADD_FRAME_TIMEOUT();
                } else        ll_err("ERROR: emitter was interrupted\n");
            } else perror("read");
        } else if(a_rcv == LL_A_SEND){
            if(c_rcv == ll_get_expected_RR()){
                ll_log(2, "    Got RR\n");
                break;
            } else if(c_rcv == ll_get_expected_REJ()){
                ADD_FRAME_ERROR();
                ll_log(2, "    Got REJ\n");
            } else ll_err("Don't know what I got; a=0x%02X, c=0x%02X\n", a_rcv, c_rcv);
        } else{
            ll_err("ERROR: c_rcv or a_rcv are not correct\n");
            ll_err("a_rcv=0x%02X (should be 0x%02X)\n", a_rcv, LL_A_SEND);
            ll_err("c_rcv=0x%02X (should be 0x%02X, 0x%02X)\n",
                c_rcv, ll_get_expected_RR(), ll_get_expected_REJ());
        }
    }
    if(attempts == ll_config.retransmissions){
        ll_err("ERROR: gave up due to timeout\n");
        return -1;
    }
    setitimer(ITIMER_REAL, &ll_timer_reset, NULL);

    ll_log(1, "Successfully wrote\n");

    return ret;
}

int llread(int port_fd, uint8_t *buffer){
    ll_log(1, "Preparing to read\n");

    if(ll_status == TRANSMITTER) return -1;

    sequence_number = (sequence_number+1)%2;
    
    ssize_t sz = -1;

    unsigned attempts;
    for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
        timeout = 0;
        setitimer(ITIMER_REAL, &ll_config.timeout, NULL);

        // Get I
        sz = ll_expect_Iframe(port_fd, (uint8_t*)buffer);

        // Validate I
        if(sz <= 0){
            if(errno == EINTR){
                ll_err("WARNING: timeout waiting for I-frame\n");
                ADD_FRAME_TIMEOUT();
            } else
            if(ll_send_REJ(port_fd)) return -1;
        } else {
            // Send RR
            if(ll_send_RR(port_fd)) return -1;
            break;
        }
    }
    if(attempts == ll_config.retransmissions){
        ll_err("ERROR: gave up due to timeout\n");
        return -1;
    }
    setitimer(ITIMER_REAL, &ll_timer_reset, NULL);

    ll_log(1, "Successfully read\n");

    return sz;
}

int llclose(int port_fd){
    ll_log(1, "Preparing to close\n");

    int res;

    if(ll_status == TRANSMITTER){  
        unsigned attempts;
        for(attempts = 0; attempts < ll_config.retransmissions; ++attempts){
            timeout = 0;
            setitimer(ITIMER_REAL, &ll_config.timeout, NULL);
            
            // Send DISC
            res = ll_send_DISC(port_fd);

            // Get DISC
            uint8_t a_rcv, c_rcv;
            res = ll_expect_Sframe(port_fd, &a_rcv, &c_rcv);
            
            // Validate DISC
            if(res){
                if(errno == EINTR){
                    if(timeout){
                        ll_err("WARNING: timeout waiting for DISC\n");
                        ADD_FRAME_TIMEOUT();
                    } else        ll_err("ERROR: interrupted\n");
                } else perror("read");
            } 
            else if(a_rcv == LL_A_RECV && c_rcv == LL_C_DISC) {
                ll_log(2, "    Got DISC\n");
                break;
            } else ll_err("ERROR: c_rcv or a_rcv are not correct\n");
        }
    
        if(attempts == ll_config.retransmissions){
            ll_err("ERROR: gave up due to timeout\n");
            return -1;
        }

        // Send UA
        res = ll_send_UA(port_fd);
        setitimer(ITIMER_REAL, &ll_timer_reset, NULL);
    }
    else{
        // Get DISC
        uint8_t a_rcv, c_rcv;
        res = ll_expect_Sframe(port_fd, &a_rcv, &c_rcv);

        if(res){
            perror("read"); return -1;
        }
        else if(a_rcv == LL_A_SEND && c_rcv == LL_C_DISC){
            ll_log(2, "    Got DISC\n");
        } else{ 
            ll_err("ERROR: c_rcv or a_rcv are not correct\n"); return -1;
        }
        // Send DISC
        res = ll_send_DISC(port_fd);


        // Get UA
        res = ll_expect_Uframe(port_fd, &a_rcv, &c_rcv);

        if(res){
            perror("read"); return -1;
        }
        else if(a_rcv == LL_A_RECV && c_rcv == LL_C_UA){
            ll_log(2, "    Got UA\n");
        } else{ 
            ll_err("ERROR: c_rcv or a_rcv are not correct\n"); return -1;
        }
    }

    // Restore initial port settings
    if(tcsetattr(port_fd, TCSAFLUSH, &oldtio) == -1) { perror("tcsetattr"); return -1; }
    // Close port
    if(close(port_fd) == -1) { perror("close"); return -1; };
    
    ll_log(1, "Closed successfully\n");

    return 0;
}

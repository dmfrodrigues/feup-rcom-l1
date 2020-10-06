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

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


// [FLAG,A,C,BCC,FLAG]
// flag = 01111110; A=11; C=11; BCC = A XOR C 
#define FLAG 0x7E
#define A 0x03
#define C 0x03
#define BCC (A^C)

char a_rcv, c_rcv;

typedef enum {
    Start,      // Start state
    Flag_RCV,   // Received flag
    A_RCV,      // Received address
    C_RCV,      // Received control byte
    BCC_OK,     // Received BCC, and it is correct
    Stop        // W
} uaStateMachine;

int timeout=0;

void alarmHandler(){
	printf("TIMEOUT\n");
	timeout=1;
}


uaStateMachine updateUAStateMachine(uaStateMachine state, char byte){
    switch (state) {
    case Start:
        switch(byte){
            case FLAG:               state = Flag_RCV; break;
            default  :               state = Start   ; break;
        } break;
    case Flag_RCV:
        switch(byte){
            case A   : a_rcv = byte; state = A_RCV   ; break;
            case FLAG:               state = Flag_RCV; break;
            default  :               state = Start   ; break;
        } break;
    case A_RCV:
        switch(byte){
            case C   : c_rcv = byte; state = C_RCV   ; break;
            case FLAG:               state = Flag_RCV; break;
            default  :               state = Start   ; break;
        } break;
    case C_RCV:
        switch(byte){
            case FLAG: state = Flag_RCV; break;
            default  : state = (byte == (a_rcv^c_rcv) ? BCC_OK : Start); break;
        } break;
    case BCC_OK:
        switch(byte){
            case FLAG: state = Stop ; break;
            default  : state = Start; break;
        } break;
    case Stop:
        state = Stop;
        break;
    default:
        fprintf(stderr, "No such state %d\n", state);
        break;
    }
    fprintf(stderr, "(debug) STATE: %d \n", state);
    return state;
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
    newtio.c_cc[VTIME]  = 0; // inter-character timer unused
    newtio.c_cc[VMIN]   = 5; // blocking read until 5 chars received

    tcflush(port_fd, TCIOFLUSH);    // flush data received but not read, and data written but not transmitted

    if(tcsetattr(port_fd, TCSANOW, &newtio) == -1) { perror("tcsetattr"); exit(-1); }

    (void) signal(SIGALRM, alarmHandler);
    
    char SET[5];

    SET[0] = FLAG;
    SET[1] = A;
    SET[2] = C;
    SET[3] = BCC;
    SET[4] = FLAG;

    uaStateMachine state = Start;
    int attempts = 0;
    
    while(attempts < 3 && state != Stop){
        
        attempts++;
        timeout = 0;
        alarm(3);
        
        // WRITE TO PORT
        
        int res = write(port_fd, SET, 5);
        fprintf(stderr, "Wrote SET to port: \"%s\" (%d bytes)\n", SET, res);

        // GET RESEND
        
        char resend_buf[5];
        
        int i = 0;
        do {
            int res = read(port_fd, resend_buf+i, 1);
            if(res > 0){
                state = updateUAStateMachine(state, resend_buf[i]);
                i++;
            }
        } while(state != Stop && !timeout && i < 5);
        
        // VALIDATION
        
        fprintf(stderr, "Got resend: \"%s\" (%d bytes)\n", resend_buf, i);
        if(state == Stop)   fprintf(stderr, "Resend is correct\n");
        else                fprintf(stderr, "Failed\n");
    }

    // RESTORE INITIAL PORT SETTINGS
    if(tcsetattr(port_fd, TCSANOW, &oldtio) == -1) { perror("tcsetattr"); exit(-1); }
    // CLOSE PORT
    close(port_fd);

    return 0;
}

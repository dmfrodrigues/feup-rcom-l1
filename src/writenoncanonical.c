/*Non-Canonical Input Processing*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int main(int argc, char** argv){
    // CHECK ARGUMENTS
    if(argc < 2){
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    // OPEN SERIAL PORT
    // O_RDWR   - Open for reading and writing
    // O_NOCTTY - Open serial port not as controlling tty, because we don't want to get killed if linenoise sends CTRL-C
    int port_fd = open(argv[1], O_RDWR | O_NOCTTY);
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

    // GET INPUT
    char *buf = malloc(255*sizeof(char)); size_t n = 255;
    fprintf(stderr, "Write here whatever you want to send the other computer:\n");
    if(getline(&buf, &n, stdin) == -1){ perror("getline"); exit(-1); }
    buf[strlen(buf)-1] = '\0';
    // WRITE TO PORT
    int res = write(port_fd, buf, strlen(buf)+1);
    fprintf(stderr, "Wrote to port: \"%s\" (%d bytes) [", buf, res);
    for(int i = 0; i < strlen(buf)+1; ++i) fprintf(stderr, "0x%02X ", buf[i]);
    fprintf(stderr, "]\n");

    // GET RESEND
    char resend_buf[255];
    int sz = 0;
    do {
        int res = read(port_fd, resend_buf+sz, 1);
    } while(buf[sz++] != '\0');
    fprintf(stderr, "Got resend   : \"%s\" (%d bytes) [", resend_buf, sz);
    for(int i = 0; i < sz; ++i) fprintf(stderr, "0x%02X ", resend_buf[i]);
    fprintf(stderr, "]\n");

    // VALIDATION
    if(strcmp(buf, resend_buf) == 0) fprintf(stderr, "Resend is correct\n");
    else                             fprintf(stderr, "Resend is not correct\n");

    // RESTORE INITIAL PORT SETTINGS
    if(tcsetattr(port_fd, TCSANOW, &oldtio) == -1) { perror("tcsetattr"); exit(-1); }
    // CLOSE PORT
    close(port_fd);

    free(buf);

    return 0;
}

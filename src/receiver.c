// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"
#include "app.h"
#include "app_args.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int main(int argc, char** argv){

    int com = 0;

    if(app_parse_args(argc, argv, &com, RECEIVER, NULL)){
        printf("Usage:\tnserial SerialPort\n\tex: nserial 2\n");
        exit(1);
    }

    if(application(com, RECEIVER, NULL) < 0)
        return 1;

    return 0;
}

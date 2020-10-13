// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll.h"
#include "app.h"
#include "app_args.h"

#define _POSIX_SOURCE 1 /* POSIX compliant source */

int main(int argc, char** argv){

    int com = 0;
    char *file_path = NULL;

    if(app_parse_args(argc, argv, &com, TRANSMITTER, &file_path)){
        printf("Usage:\tnserial SerialPort FileName\n\tex: ./transmitter 2 pinguim.gif\n");
        exit(1);
    }
    
    if(application(com, TRANSMITTER, file_path) < 0)
        return 1;

    return 0;
}

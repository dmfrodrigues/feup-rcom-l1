// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup app Application layer
 * @ingroup ll 
 * @brief Application layer
 */

#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include "ll.h"

#define CTRL_DATA   1
#define CTRL_START  2
#define CTRL_END    3

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

#define FILE_NAME_MAX_SIZE 255

typedef struct {
    int fileDescriptor; ///< Descriptor corresponding to the serial port
    ll_status_t status; ///< TRANSMITTER | RECEIVER
} application_layer;

int application(int com, ll_status_t status, char *file_path);

int app_send_ctrl_packet(int ctrl, uint32_t file_size, const char *file_name);

int app_send_data_packet(char*data, unsigned int data_size, unsigned int seq_number);

int app_send_file(char *file_path);

int app_rcv_ctrl_packet(int ctrl, unsigned int * file_size, char * file_name);

int app_rcv_data_packet(char * data, int seq_number);

int app_receive_file(void);

#endif // _APPLICATION_LAYER_H_

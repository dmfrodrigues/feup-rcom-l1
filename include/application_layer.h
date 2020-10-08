#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

#include <stdio.h>
#include "ll.h"

#define T_FILE_SIZE 0
#define T_FILE_NAME 1
#define CTRL_START 2
#define CTRL_END 3

typedef struct {
    int fileDescriptor; /*Descritor correspondente à porta série*/
    ll_status_t status; /*TRANSMITTER | RECEIVER*/
    ll_config_t ll_config;
} application_layer;

int application(int port_fd, ll_status_t status, int baud_rate, unsigned int timeout, unsigned int retransmissions);

int app_send_ctrl_packet(int ctrl, size_t file_size, char *file_name);

int app_send_data();

int app_receive_data();

#endif // _APPLICATION_LAYER_H_

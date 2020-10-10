#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "ll.h"

#define CTRL_DATA   1
#define CTRL_START  2
#define CTRL_END    3

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

#define FILE_NAME_MAX_SIZE 255

typedef struct {
    int fileDescriptor; /*Descritor correspondente à porta série*/
    ll_status_t status; /*TRANSMITTER | RECEIVER*/
} application_layer;

int application(int com, ll_status_t status, const char *file_name);

int app_send_ctrl_packet(int ctrl, uint32_t file_size, const char *file_name);

int app_send_data_packet(uint8_t *data, size_t data_size, unsigned int seq_number);

int app_send_file(const char *file_name);

int app_rcv_ctrl_packet(int ctrl, int * file_size, char * file_name);

int app_rcv_data_packet(uint8_t * data, int seq_number);

int app_receive_file(void);

#endif // _APPLICATION_LAYER_H_

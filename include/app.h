// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup app Application layer
 * @brief Application layer
 */

#ifndef _APP_H_
#define _APP_H_

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

/**
 * @ingroup app
 * @brief App configuration structure
 */
typedef struct {
    int fileDescriptor;
    size_t packet_size;
} app_config_t;

/**
 * @ingroup app
 * @brief App configuration
 */
app_config_t app_config;

/**
 * @ingroup app
 * @brief Initializes the application.
 * 
 * @param com           Port to connect to
 * @param status        Status of the application, TRANSMITTER or RECEIVER
 * @param file_path     Path for the file to be transmitted, NULL if it is RECEIVER
 * @return int          0 on success, -1 on error
 */
int application(int com, ll_status_t status, char *file_path)
    __attribute__((warn_unused_result));

/**
 * @ingroup app
 * @brief Sends control packet, indicating the beginning or end of the file transfer.
 * 
 * @param ctrl          Control field
 * @param file_size     Size of the file to be trasmitted
 * @param file_path     Path of the file to be transmitted
 * @return int          0 on success; -1 on error
 */
int app_send_ctrl_packet(int ctrl, uint32_t file_size, const char *file_name)
    __attribute__((warn_unused_result));

/**
 * @ingroup app
 * @brief Sends data packet.
 * 
 * @param data          Piece of data from the file to be sent
 * @param data_size     Size of the data in bytes
 * @param seq_number    Sequence number, used to verify the integrity of data
 * @return int          If successful, returns the number of bytes sent; -1 on error
 */
int app_send_data_packet(char * data, unsigned int data_size, unsigned int seq_number)
    __attribute__((warn_unused_result));

/**
 * @ingroup app
 * @brief Sends file.
 * 
 * @param file_path     Path of the file to be transmitted
 * @return int          0 on success; -1 on error
 */
int app_send_file(char * file_path)
    __attribute__((warn_unused_result));
/**
 * @ingroup app
 * @brief Receives control packet.
 * 
 * @param ctrl          Expected control field
 * @param file_size     Pointer where the file_size will be saved
 * @param file_path     Pointer where the file_name will be saved
 * @return int          0 on success; -1 on error
 */
int app_rcv_ctrl_packet(int ctrl, unsigned int * file_size, char * file_name)
    __attribute__((warn_unused_result));

/**
 * @ingroup app
 * @brief Receives data packet.
 * 
 * @param data          Pointer where the data will be stored
 * @param seq_number    Expected sequence number
 * @return int          0 on success; -1 on error
 */
int app_rcv_data_packet(char * data, int seq_number)
    __attribute__((warn_unused_result));

/**
 * @ingroup app
 * @brief Receives file.
 * 
 * @return int          0 on success; -1 on error
 */
int app_receive_file(void)
    __attribute__((warn_unused_result));

#endif // _APP_H_

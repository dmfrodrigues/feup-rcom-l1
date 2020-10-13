// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup app_args Application layer argument parsing
 * @ingroup app 
 * @brief Application layer argument parsing
 */

#ifndef _APP_ARGS_H_
#define _APP_ARGS_H_

#include "ll.h"

/**
 * @ingroup app_args
 * @brief Parse app arguments
 * 
 * @param argc      Argument count
 * @param argv      Argument vector
 * @param com       Pointer to int to which COM will be stored
 * @param status    Status (TRANSMITTER or RECEIVER)
 * @param file_path Pointer to string where file path to transfer will be stored.
 *                  If status is RECEIVER, file_path is not used and should be set to NULL
 * @return int      0 if success, 1 otherwise
 */
int app_parse_args(int argc, char *argv[], int *com, ll_status_t status, char **file_path)
    __attribute__((warn_unused_result));

#endif // _APP_ARGS_H_

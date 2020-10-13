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

int app_parse_args(int argc, char *argv[], int *com, ll_status_t status, char **file_path)
    __attribute__((warn_unused_result));

#endif // _APP_ARGS_H_

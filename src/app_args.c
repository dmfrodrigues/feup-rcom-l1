// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "app_args.h"

#include "app.h"

#include <getopt.h>
#include <stdlib.h>

#include "stats.h"

static const char optstring[] = "b:d:h:r:s:t:v:";

static const struct option longopts[] = {
    {"baudrate"       , 1, NULL, 'b'},
    {"prob-error-data", 1, NULL, 'd'},
    {"prob-error-head", 1, NULL, 'h'},
    {"retransmissions", 1, NULL, 'r'},
    {"size"           , 1, NULL, 's'},
    {"timeout"        , 1, NULL, 't'},
    {"verbosity"      , 1, NULL, 'v'},
    {0,0,0,0}
};

int app_parse_args(int argc, char *argv[], int *com, ll_status_t status, char **file_path){
    int res = EXIT_SUCCESS;

    opterr = 0;
    optind = 1;
    int opt = 0; int longindex;
    while((opt = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1){
        switch(opt){
            case 'b': if(sscanf(optarg, "%lu", &ll_config.baud_rate         ) != 1) res = EXIT_FAILURE; break;
            case 'd': if(sscanf(optarg, "%f" , &stats_config.prob_error_data) != 1) res = EXIT_FAILURE; break;
            case 'h': if(sscanf(optarg, "%f" , &stats_config.prob_error_head) != 1) res = EXIT_FAILURE; break;
            case 'r': if(sscanf(optarg, "%u" , &ll_config.retransmissions   ) != 1) res = EXIT_FAILURE; break;
            case 's': if(sscanf(optarg, "%lu", &app_config.packet_size      ) != 1) res = EXIT_FAILURE; break;
            case 't': if(sscanf(optarg, "%u" , &ll_config.timeout           ) != 1) res = EXIT_FAILURE; break;
            case 'v': if(sscanf(optarg, "%d" , &ll_config.verbosity         ) != 1) res = EXIT_FAILURE; break;
            default : res = 1;
        }
    }

    if(status == TRANSMITTER){
        if(optind != argc-2) res = 1;
        else {
            *com = atoi(argv[optind]);
            *file_path = argv[optind+1];
        }
    } else if(status == RECEIVER){
        if(optind != argc-1) res = 1;
        else {
            *com = atoi(argv[optind]);
        }
    }

    return res;
}

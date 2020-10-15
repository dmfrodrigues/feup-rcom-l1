#include "stats.h"

#include <stdio.h>
#include <stdlib.h>

#include "ll.h"
#include "ll_utils.h"

#define SECONDS_TO_MICROS 1000000

stats_t stats = {
    0,
    0,
    0,
    0,
    0,
    &ll_config.baud_rate,
    0
};

stats_config_t stats_config = {
    0.0,
    0.0,
    0
};

struct timeval time_start, time_end;

void tic(void){
    gettimeofday(&time_start, NULL);
}

void toc(void){
    gettimeofday(&time_end, NULL);
    stats.T = (time_end.tv_sec  - time_start.tv_sec )*SECONDS_TO_MICROS +
              (time_end.tv_usec - time_start.tv_usec);
}

void print_stats(void){
    ll_log(1, "Statistics:\n");
    ll_log(1, "    L : %lu\n", stats.L ); fprintf(stdout, "%lu\n", stats.L );
    ll_log(1, "    Lf: %lu\n", stats.Lf); fprintf(stdout, "%lu\n", stats.Lf);
    ll_log(1, "    N : %lu\n", stats.N ); fprintf(stdout, "%lu\n", stats.N );
    ll_log(1, "    Ne: %lu\n", stats.Ne); fprintf(stdout, "%lu\n", stats.Ne);
    ll_log(1, "    Nt: %lu\n", stats.Nt); fprintf(stdout, "%lu\n", stats.Nt);
    ll_log(1, "    C : %lu\n",*stats.C ); fprintf(stdout, "%lu\n",*stats.C );
    ll_log(1, "    T : %lu\n", stats.T ); fprintf(stdout, "%lu\n", stats.T );
}

void gen_frame_error(float prob, uint8_t *frame, size_t frame_size){
    for(size_t i = 0; i < 8*frame_size; ++i){
        size_t idx = i/8, bit = i%8;
        float rand_value = rand() / (float) RAND_MAX;
        if(rand_value < prob) {
            ll_log(3, "        Generating a random bit swap\n");
            frame[idx] ^= (1 << bit);
        }
    }
}

void add_delay(useconds_t usec){
    ll_log(3, "        Adding delay of %d microseconds\n", usec);
    usleep(usec);
}
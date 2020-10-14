#include "stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ll.h"
#include "ll_utils.h"

#define SECONDS_TO_MICROS 1000000

stats_t stats = {
    L : 0,
    Lf: 0,
    N : 0,
    Ne: 0,
    Nt: 0,
    C : &ll_config.baud_rate,
    T : 0
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
    fprintf(stderr, "Statistics:\n");
    fprintf(stderr, "    L : %lu\n", stats.L );
    fprintf(stderr, "    Lf: %lu\n", stats.Lf);
    fprintf(stderr, "    N : %lu\n", stats.N );
    fprintf(stderr, "    Ne: %lu\n", stats.Ne);
    fprintf(stderr, "    Nt: %lu\n", stats.Nt);
    fprintf(stderr, "    C : %lu\n",*stats.C );
    fprintf(stderr, "    T : %lu\n", stats.T );
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

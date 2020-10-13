#include "stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ll.h"

#define SECONDS_TO_MICROS 1000000

stats_t stats = {
    L : 0,
    Lf: 0,
    N : 0,
    Ne: 0,
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
    fprintf(stderr, "    C : %lu\n",*stats.C );
    fprintf(stderr, "    T : %lu\n", stats.T );
}

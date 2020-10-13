/**
 * @defgroup stats Statistics
 */
#ifndef _STATS_H_
#define _STATS_H_

#include <stdlib.h>
#include <sys/time.h>

typedef struct {
    size_t L;
    size_t Lf;
    size_t N;
    size_t Ne;
    size_t *C;
    suseconds_t T;    
} stats_t;

stats_t stats;

void tic(void);
void toc(void);

void print_stats(void);

#ifdef STATISTICS
    #define ADD_MESSAGE_LENGTH(length)  {stats.L  += (length); }
    #define ADD_FILE_LENGTH(length)     {stats.Lf += (length); }
    #define ADD_FRAME()                 {++stats.N           ; }
    #define ADD_FRAME_ERROR()           {++stats.Ne          ; }
    #define TIC()                       tic()
    #define TOC()                       toc()
    #define PRINT_STATS()               print_stats()
#else
    #define ADD_MESSAGE_LENGTH(length)
    #define ADD_FILE_LENGTH(length)
    #define ADD_FRAME()
    #define ADD_FRAME_ERROR()
    #define TIC()
    #define TOC()
    #define PRINT_STATS()
#endif

#endif // _STATS_H_

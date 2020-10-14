/**
 * @defgroup stats Statistics
 */
#ifndef _STATS_H_
#define _STATS_H_

#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct {
    size_t L;
    size_t Lf;
    size_t N;
    size_t Ne;
    size_t Nt;
    size_t *C;
    suseconds_t T;    
} stats_t;

stats_t stats;

void tic(void);
void toc(void);

void print_stats(void);

/**
 * @ingroup stats
 * @brief Generates random bit swaps.
 * 
 * If the probability is negative or zero, no errors will be generated
 * 
 * @param prob          Value from 0 to 1, meaning the probability of a bit swap happening to a bit
 * @param frame         Frame where the error will be generated
 * @param frame_size    Size of frame
 */
void gen_frame_error(float prob, uint8_t *frame, size_t frame_size);

#ifdef STATISTICS
    #define ADD_MESSAGE_LENGTH(length)                  {stats.L  += (length); }
    #define ADD_FILE_LENGTH(length)                     {stats.Lf += (length); }
    #define ADD_FRAME()                                 {++stats.N           ; }
    #define ADD_FRAME_ERROR()                           {++stats.Ne          ; }
    #define ADD_FRAME_TIMEOUT()                         {++stats.Nt          ; }
    #define TIC()                                       tic()
    #define TOC()                                       toc()
    #define PRINT_STATS()                               print_stats()
    #ifdef STATISTICS_ERRORS
        #define GEN_FRAME_ERROR(prob, frame, frame_size)    gen_frame_error(prob, frame, frame_size)
    #else
        #define GEN_FRAME_ERROR(prob, frame, frame_size)
    #endif
#else
    #define ADD_MESSAGE_LENGTH(length)
    #define ADD_FILE_LENGTH(length)
    #define ADD_FRAME()
    #define ADD_FRAME_ERROR()
    #define ADD_FRAME_TIMEOUT()
    #define TIC()
    #define TOC()
    #define PRINT_STATS()
    #define GEN_FRAME_ERROR(prob, frame, frame_size)
#endif

#endif // _STATS_H_

// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup    stats Statistics
 * @brief       Statistics module
 */

/**
 * @defgroup    stats_integrated Statistics, integrated part
 * @ingroup     stats
 * @brief       Statistics module, integrated part
 * 
 * The macros documented under this submodule are only defined if the
 * compilation flag STATISTICS is defined. Additionally, ADD_DELAY requires
 * flag STATISTICS_DELAY to be defined, and GEN_FRAME_ERROR requires
 * flag STATISTICS_ERRORS to be defined.
 */

/**
 * @defgroup    stats_separate Statistics, separate part
 * @ingroup     stats
 * @brief       Statistics module, separate part
 */

#ifndef _STATS_H_
#define _STATS_H_

#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

/**
 * @ingroup stats_integrated
 * @brief Statistics data structure.
 */
typedef struct {
    size_t L;       ///< Total message length, including frames with errors (in bytes)
    size_t Lf;      ///< File length (in bytes)
    size_t N;       ///< Total number of frames
    size_t Ne;      ///< Number of frames with errors
    size_t Nt;      ///< Number of frames that resulted in timeout
    size_t *C;      ///< Capacity (in bits/second)
    suseconds_t T;  ///< Execution time (in microseconds)
} stats_t;

/**
 * @ingroup stats_integrated
 * @brief Statistics data object.
 */
stats_t stats;

/**
 * @ingroup stats_integrated
 * @brief Statistics configuration structure.
 */
typedef struct {
    float prob_error_head;  ///< Probability of header bit randomly swapping
    float prob_error_data;  ///< Probability of data bit randomly swapping
    useconds_t dtau;        ///< Artificial signal propagation delay
} stats_config_t;

/**
 * @ingroup stats_integrated
 * @brief Statistics configuration object.
 */
stats_config_t stats_config;

/**
 * @ingroup stats_integrated
 * @brief Start timer.
 * 
 * @see toc()
 */
void tic(void);

/**
 * @ingroup stats_integrated
 * @brief Register current time.
 * 
 * The time registered by this function can be consulted at stats.T (in microseconds)
 * 
 * @see tic()
 */
void toc(void);

void print_stats(void);

/**
 * @ingroup stats
 * @brief Generates random bit swaps.
 * 
 * If the probability is negative or zero, no errors will be generated
 * 
 * @param prob          Value from 0 to 1, meaning the probability of a bit swap
 *                      happening to a bit
 * @param frame         Frame where the error will be generated
 * @param frame_size    Size of frame
 */
void gen_frame_error(float prob, uint8_t *frame, size_t frame_size);

/**
 * @ingroup stats_integrated
 * @brief Generate artificial delay (sleep).
 * 
 * @param usec Time to delay (in microseconds)
 */
void add_delay(useconds_t usec);

#ifdef STATISTICS
    #define ADD_MESSAGE_LENGTH(length)                  {stats.L  += (length); }
    #define ADD_FILE_LENGTH(length)                     {stats.Lf += (length); }
    #define ADD_FRAME()                                 {++stats.N           ; }
    #define ADD_FRAME_ERROR()                           {++stats.Ne          ; }
    #define ADD_FRAME_TIMEOUT()                         {++stats.Nt          ; }
    #define TIC()                                       tic()
    #define TOC()                                       toc()
    #define PRINT_STATS()                               print_stats()
    #ifdef STATISTICS_DELAY
        #define ADD_DELAY(usec)                             add_delay(usec)
    #else
        #define ADD_DELAY(usec)                             {}
    #endif
    #ifdef STATISTICS_ERRORS
        #define GEN_FRAME_ERROR(prob, frame, frame_size)    \
            gen_frame_error(prob, frame, frame_size)
    #else
        #define GEN_FRAME_ERROR(prob, frame, frame_size)    {}
    #endif
#else
    /// @ingroup stats_integrated @brief Add to total message length
    #define ADD_MESSAGE_LENGTH(length)                  {}
    /// @ingroup stats_integrated @brief Add to file length
    #define ADD_FILE_LENGTH(length)                     {}
    /// @ingroup stats_integrated @brief Increment number of frames
    #define ADD_FRAME()                                 {}
    /// @ingroup stats_integrated @brief Increment number of frame errors
    #define ADD_FRAME_ERROR()                           {}
    /// @ingroup stats_integrated @brief Increment number of frame timeouts
    #define ADD_FRAME_TIMEOUT()                         {}
    /// @ingroup stats_integrated @brief Wrapper of tic()
    #define TIC()                                       {}
    /// @ingroup stats_integrated @brief Wrapper of toc()
    #define TOC()                                       {}
    /// @ingroup stats_integrated @brief Wrapper of print_stats()
    #define PRINT_STATS()                               {}
    /// @ingroup stats_integrated @brief Wrapper of gen_frame_error(float, uint8_t*, size_t)
    #define GEN_FRAME_ERROR(prob, frame, frame_size)    {}
    /// @ingroup stats_integrated @brief Wrapper of add_delay(useconds_t)
    #define ADD_DELAY(usec)                             {}
#endif

#endif // _STATS_H_

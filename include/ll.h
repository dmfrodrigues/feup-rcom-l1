#define LL_MAX_SIZE 1024

#include "ll_utils.h"

typedef struct {
    int baud_rate;
    unsigned int timeout;
    unsigned int retransmissions;
} ll_config_t;

typedef enum {
    TRANSMITTER,
    RECEIVER
} ll_status_t;

ll_config_t ll_config;

/**
 * @brief Open serial port.
 * 
 * Since there is usually COM1-COM4, you should pass a value 1-4,
 * although you can use /dev/ttyS10 (COM11) by passing 11 as com.
 * 
 * @param com       Port to connect to.
 * @param status    Whether the caller is a transmitter or receiver
 * @return int      ID of connection, or a negative value if an error occured.
 */
int llopen(int com, ll_status_t status) __attribute__((warn_unused_result));

/**
 * @brief Close serial port.
 * 
 * @param id        Port to close.
 * @return int      0 if successful, other value otherwise
 */
int llclose(int id) __attribute__((warn_unused_result));

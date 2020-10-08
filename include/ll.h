/**
 * @defgroup    ll  LL
 * @brief       Logical link (LL) protocol.
 */

#define LL_MAX_SIZE 1024        ///< @ingroup ll Maximum size for a message to be sent using llwrite

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
 * @ingroup ll
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
 * @ingroup ll
 * @brief Write to serial port.
 * 
 * @param id        Port to write to.
 * @param buffer    Data to be written to port.
 * @param length    Length of data to be written to port.
 * @return int      Number of written characters, or a negative value if error.
 */
int llwrite(int id, const char *buffer, int length) __attribute__((warn_unused_result));

/**
 * @ingroup ll
 * @brief Read serial port.
 * 
 * @param id        Port to read.
 * @param buffer    Data to be read.
 * @return int      Number of read characters, or a negative value if error.
 */
int llread(int id, const char *buffer) __attribute__((warn_unused_result));

/**
 * @ingroup ll
 * @brief Close serial port.
 * 
 * @param id        Port to close.
 * @return int      positive value if successful, negative otherwise
 */
int llclose(int id) __attribute__((warn_unused_result));

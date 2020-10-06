#include <stdint.h>

const uint8_t FLAG   = 0x7E; // FLAG
const uint8_t A_SEND = 0x03; // Address of sender; used on sender messages, or receiver responses
const uint8_t A_RECV = 0x01; // Address of receiver; used on receiver messages, or sender responses
const uint8_t C_SET  = 0x03; // Control: setup
const uint8_t C_DISC = 0x0B; // Control: disconnect
const uint8_t C_UA   = 0x07; // Control: unnumbered acknowledgement
const uint8_t ESC    = 0x7D; // Escape FLAG

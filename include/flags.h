#include <stdint.h>

#define SP_FLAG    0x7E // FLAG
#define SP_A_SEND  0x03 // Address of sender; used on sender messages, or receiver responses
#define SP_A_RECV  0x01 // Address of receiver; used on receiver messages, or sender responses
#define SP_C_SET   0x03 // Control: setup
#define SP_C_DISC  0x0B // Control: disconnect
#define SP_C_UA    0x07 // Control: unnumbered acknowledgement
#define SP_ESC     0x7D // Escape FLAG

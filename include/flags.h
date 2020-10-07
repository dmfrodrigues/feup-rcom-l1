#include <stdint.h>

#define SP_FLAG    0x7E // FLAG
#define SP_A_SEND  0x03 // Address of sender; used on sender messages, or receiver responses
#define SP_A_RECV  0x01 // Address of receiver; used on receiver messages, or sender responses
#define SP_C_SET   0x03 // Control: setup
#define SP_C_DISC  0x0B // Control: disconnect
#define SP_C_UA    0x07 // Control: unnumbered acknowledgement
#define SP_ESC     0x7D // Escape FLAG

#define LL_C(N)    (N << 6)

#define SP_RR_MASK  0x05 // Receiver ready mask
#define SP_REJ_MASK 0x01 // Reject mask
#define LL_RR(N)   (SP_RR_MASK  | (N << 7))
#define LL_REJ(N)  (SP_REJ_MASK | (N << 7))

#define LL_ESCAPE(c) (c ^ 0x20)
#define LL_DEESCAPE(c) (c ^ 0x20)

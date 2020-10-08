#include <stdint.h>

/**
 * @defgroup    ll_flags LL flags
 * @ingroup     ll
 * @brief       Flags for Logical Link (LL).
 */

#define SP_FLAG         0x7E                        ///< @ingroup ll_flags FLAG
#define SP_A_SEND       0x03                        ///< @ingroup ll_flags Address of sender; used on sender messages, or receiver responses
#define SP_A_RECV       0x01                        ///< @ingroup ll_flags Address of receiver; used on receiver messages, or sender responses
#define SP_C_SET        0x03                        ///< @ingroup ll_flags Control: setup
#define SP_C_DISC       0x0B                        ///< @ingroup ll_flags Control: disconnect
#define SP_C_UA         0x07                        ///< @ingroup ll_flags Control: unnumbered acknowledgement
#define SP_ESC          0x7D                        ///< @ingroup ll_flags Escape FLAG

#define LL_C(N)         (N << 6)                    ///< @ingroup ll_flags Get control (C) byte from sequence number (aka Ns or Nr)

#define SP_RR_MASK      0x05                        ///< @ingroup ll_flags Receiver ready mask
#define SP_REJ_MASK     0x01                        ///< @ingroup ll_flags Reject mask
#define LL_RR(N)        (SP_RR_MASK  | (N << 7))    ///< @ingroup ll_flags Get Receiver Ready (RR) flag from sequence number
#define LL_REJ(N)       (SP_REJ_MASK | (N << 7))    ///< @ingroup ll_flags Get Rejected (REJ) flag from sequence number

#define LL_ESCAPE(c)    (c ^ 0x20)                  ///< @ingroup ll_flags Escape character
#define LL_DEESCAPE(c)  (c ^ 0x20)                  ///< @ingroup ll_flags Deescape character

// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

/**
 * @defgroup    ll_flags LL flags
 * @ingroup     ll
 * @brief       Flags for Logical Link (LL).
 */

#ifndef _LL_FLAGS_H_
#define _LL_FLAGS_H_

#include <stdint.h>


#define LL_FLAG         0x7E                        ///< @ingroup ll_flags FLAG
#define LL_A_SEND       0x03                        ///< @ingroup ll_flags Address of sender
#define LL_A_RECV       0x01                        ///< @ingroup ll_flags Address of receiver
#define LL_C_SET        0x03                        ///< @ingroup ll_flags Control: setup
#define LL_C_DISC       0x0B                        ///< @ingroup ll_flags Control: disconnect
#define LL_C_UA         0x07                        ///< @ingroup ll_flags Control: unnumbered acknowledgement
#define LL_ESC          0x7D                        ///< @ingroup ll_flags Escape FLAG

#define LL_C(N)         ((N) << 6)                  ///< @ingroup ll_flags Get C byte from sequence number

#define LL_RR_MASK      0x05                        ///< @ingroup ll_flags Receiver ready mask
#define LL_REJ_MASK     0x01                        ///< @ingroup ll_flags Reject mask
#define LL_RR(N)        (LL_RR_MASK  | ((N) << 7))  ///< @ingroup ll_flags Get RR flag from sequence number
#define LL_REJ(N)       (LL_REJ_MASK | ((N) << 7))  ///< @ingroup ll_flags Get REJ flag from sequence number

#define LL_STUFF(c)     ((c) ^ 0x20)                ///< @ingroup ll_flags Bit stuff a character

#endif // _LL_FLAGS_H_

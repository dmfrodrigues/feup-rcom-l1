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


#define LL_FLAG         0x7E                        ///< @ingroup ll_flags @brief FLAG
#define LL_A_SEND       0x03                        ///< @ingroup ll_flags @brief Address of sender
#define LL_A_RECV       0x01                        ///< @ingroup ll_flags @brief Address of receiver
#define LL_C_SET        0x03                        ///< @ingroup ll_flags @brief Control: setup
#define LL_C_DISC       0x0B                        ///< @ingroup ll_flags @brief Control: disconnect
#define LL_C_UA         0x07                        ///< @ingroup ll_flags @brief Control: unnumbered acknowledgement
#define LL_ESC          0x7D                        ///< @ingroup ll_flags @brief Escape FLAG

#define LL_C(N)         ((N) << 6)                  ///< @ingroup ll_flags @brief Get C byte from sequence number

#define LL_RR_MASK      0x05                        ///< @ingroup ll_flags @brief Receiver ready mask
#define LL_REJ_MASK     0x01                        ///< @ingroup ll_flags @brief Reject mask
#define LL_RR(N)        (LL_RR_MASK  | ((N) << 7))  ///< @ingroup ll_flags @brief Get RR flag from sequence number
#define LL_REJ(N)       (LL_REJ_MASK | ((N) << 7))  ///< @ingroup ll_flags @brief Get REJ flag from sequence number

#define LL_STUFF(c)     ((c) ^ 0x20)                ///< @ingroup ll_flags @brief Bit stuff a character

#endif // _LL_FLAGS_H_

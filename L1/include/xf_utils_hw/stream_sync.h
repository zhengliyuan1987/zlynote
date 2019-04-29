#ifndef XF_UTILS_HW_STRM_SYNC_H
#define XF_UTILS_HW_STRM_SYNC_H

#include "xf_utils_hw/types.h"

/**
 * @file stream_sync.h
 * @brief Barrier-like logic.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Synchronize streams for successor module.
 * 
 * This module ensures that elements in different input stream goes into
 * successor stream in locked steps. The end signal is also fused into one.
 * It assumes the input elements in each input stream have _the same number_,
 * otherwise, it will not terminate.
 */
template <typename _TIn, int _NStrm>
void stream_sync(hls::stream<_TIn> istrms[_NStrm],
                 hls::stream<bool> e_istrms[_NStrm],
                 hls::stream<_TIn> ostrms[_NStrm],
                 hls::stream<bool> e_ostrm);

} // utils_hw
} // common
} // xf

// TODO implementation
// ...

#endif // XF_UTILS_HW_STRM_SYNC_H

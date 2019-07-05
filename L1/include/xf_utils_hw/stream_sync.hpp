#ifndef XF_UTILS_HW_STRM_SYNC_H
#define XF_UTILS_HW_STRM_SYNC_H

#include "xf_utils_hw/types.hpp"

/**
 * @file stream_sync.hpp
 * @brief Barrier-like logic.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ===============================================================

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
 *
 * @tparam _TIn input type.
 * @tparam _NStrm number of input streams.
 *
 * @param istrms input data streams.
 * @param e_istrms end flags for each input.
 * @param ostrms output data streams.
 * @param e_ostrm end flag for all output.
 */
template <typename _TIn, int _NStrm>
void stream_sync(hls::stream<_TIn> istrms[_NStrm],
                 hls::stream<bool> e_istrms[_NStrm],
                 hls::stream<_TIn> ostrms[_NStrm],
                 hls::stream<bool>& e_ostrm);

} // utils_hw
} // common
} // xf

// Implementation =============================================================

namespace xf {
namespace common {
namespace utils_hw {

template <typename _TIn, int _NStrm>
void stream_sync(hls::stream<_TIn> istrms[_NStrm],
                 hls::stream<bool> e_istrms[_NStrm],
                 hls::stream<_TIn> ostrms[_NStrm],
                 hls::stream<bool>& e_ostrm) {
    ap_uint<_NStrm> last = 0;
    ap_uint<_NStrm> end = ~last;
    for (int i = 0; i < _NStrm; ++i) {
#pragma HLS unroll
        last[i] = e_istrms[i].read();
    }
    while (last != end) {
#pragma HLS pipeline II = 1
        for (int i = 0; i < _NStrm; ++i) {
#pragma HLS unroll
            _TIn d = istrms[i].read();
            ostrms[i].write(d);
            last[i] = e_istrms[i].read();
        }
        e_ostrm.write(false);
    } // while
    e_ostrm.write(true);
}

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_STRM_SYNC_H

#ifndef XF_UTILS_HW_STREAM_SHUFFLE_H
#define XF_UTILS_HW_STREAM_SHUFFLE_H

#include "xf_utils_hw/types.h"

/**
 * @file stream_shuffle.h
 * @brief Unidirectional cross-bar of streams.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ===============================================================

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Shuffle the contents from an array of streams to another.
 *
 * Suppose we have an array of 3 streams for R-G-B channels correspndingly,
 * and it is needed to shuffle B to Stream 0 and R to Stream 2.
 * This module can bridge this case with the configuration ``2, 1, 0``.
 * Here, ``2`` is the new index for data at old stream index ``0``,
 * and ``0`` is the new index for data at old stream index ``2``.
 *
 * The configuration is load once in one invocation, and reused until the end.
 * Totally ``_NStrm`` index integers will be read.
 *
 * @tparam _TIn input type.
 * @tparam _NStrm number of output stream.
 *
 * @param order_cfg the new order within the window, indexed from 0.
 * @param istrms input data streams.
 * @param e_istrm end flags for input.
 * @param ostrms output data streams.
 * @param e_ostrm end flag for output.
 */
template <typename _TIn, int _NStrm>
void stream_shuffle(hls::stream<int> order_cfg,

                    hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool> e_istrm,

                    hls::stream<_TIn> ostrms[_NStrm],
                    hls::stream<bool> e_ostrm); // TODO

} // utils_hw
} // common
} // xf

// Implementation =============================================================



#endif // XF_UTILS_HW_STREAM_SHUFFLE_H

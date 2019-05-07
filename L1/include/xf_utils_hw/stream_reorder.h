#ifndef XF_UTILS_HW_STRM_REORDER_H
#define XF_UTILS_HW_STRM_REORDER_H

#include "xf_utils_hw/types.h"

/**
 * @file stream_reoder.h
 * @brief utility to do window-reorder on a stream.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ===============================================================

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Window-reorder in a stream.
 *
 * Suppose 8bit RGB values are multiplexed into one stream in R-G-B order,
 * and a process module expects channel values in B-G-R order,
 * then the data needs to be reordered in a window with size 3,
 * and the configuration would be ``2, 1, 0``.
 *
 * It is assumed that _the total number of elements passed through this module
 * is multiple of window-size_. Otherwise, the module may hang in execution.
 *
 * The configuration is load once in one invocation, and reused until the end.
 * Totally ``_WindowSize`` index integers will be read.
 *
 * @tparam _TIn input type.
 * @tparam _WindowSize size of reorder window.
 *
 * @param order_cfg the new order within the window, indexed from 0.
 * @param istrm input data stream.
 * @param e_istrm end flags for input.
 * @param ostrm output data stream.
 * @param e_ostrm end flag for output.
 */
template <typename _TIn, int _WindowSize>
void stream_reorder(hls::stream<int> order_cfg,

                    hls::stream<_TIn> istrm,
                    hls::stream<bool> e_istrm,

                    hls::stream<_TIn> ostrm,
                    hls::stream<bool> e_ostrm); // TODO

} // utils_hw
} // common
} // xf

// Implementation =============================================================

#endif // XF_UTILS_HW_STRM_REORDER_H

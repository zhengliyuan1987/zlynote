#ifndef XF_UTILS_HW_STREAM_SPLIT_H
#define XF_UTILS_HW_STREAM_SPLIT_H

#include "xf_utils_hw/types.h"
#include "xf_utils_hw/enums.h"

/**
 * @file stream_split.h
 * @brief split one streams into multiple narrow ones.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ======================================================

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief split one wide stream into multiple streams, start from the LSB.
 *
 * If ``_WIn > _WOut * _NStrm``, the extra bits will be discarded.
 *
 * @tparam _WIn input stream width, should be no less than _WOut * _NStrm.
 * @tparam _WOut output stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param istrm input data stream.
 * @param e_istrm end flag for the input.
 * @param ostrms output data streams.
 * @param e_ostrm end flag streams for all outputs.
 * @param alg algorithm selector for this function.
 */
template <int _WOut, int _WIn, int _NStrm>
void stream_split(hls::stream<ap_uint<_WIn> >& istrm,
                  hls::stream<bool>& e_istrm,

                  hls::stream<ap_uint<_WOut> > ostrms[_NStrm],
                  hls::stream<bool>& e_ostrm,

                  lsb_side_t alg); // TODO

/**
 * @brief split one wide stream into multiple streams, start from the MSB.
 *
 * If ``_WIn > _WOut * _NStrm``, the extra bits will be discarded.
 *
 * @tparam _WIn input stream width, should be no less than _WOut * _NStrm.
 * @tparam _WOut output stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param istrm input data stream.
 * @param e_istrm end flag for the input.
 * @param ostrms output data streams.
 * @param e_ostrm end flag streams for all outputs.
 * @param alg algorithm selector for this function.
 */

template <int _WOut, int _WOut, int _NStrm>
void stream_split(hls::stream<ap_uint<_WIn> >& istrm,
                  hls::stream<bool>& e_istrm,

                  hls::stream<ap_uint<_WOut> > ostrms[_NStrm],
                  hls::stream<bool>& e_ostrm,

                  msb_side_t alg); // TODO

} // utils_hw
} // common
} // xf

// Implementation ====================================================

namespace xf {
namespace common {
namespace utils_hw {} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_STREAM_SPLIT_H

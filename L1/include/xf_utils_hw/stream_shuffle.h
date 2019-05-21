#ifndef XF_UTILS_HW_STREAM_SHUFFLE_H
#define XF_UTILS_HW_STREAM_SHUFFLE_H

#include "xf_utils_hw/types.h"
#include "xf_utils_hw/common.h"

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
 * Suppose we have an array of 3 streams for R-G-B channels correspondingly,
 * and it is needed to shuffle B to Stream 0 and R to Stream 2.
 * This module can bridge this case with the configuration ``2, 1, 0``.
 * Here, ``2`` is the new index for data at old stream index ``0``,
 * and ``0`` is the new index for data at old stream index ``2``.
 *
 * If ``-1`` is used as the destination index, the stream will be discarded.
 * Other minus value is illegal.
 *
 * If an output index is not specified, corresponding stream will be filled
 * with zero. If an output index is specified twice, the behavior is undefined.
 *
 * The configuration is load once in one invocation, and reused until the end.
 * This module supports up to 128 input streams, and works efficiently within
 * 16.
 *
 * @tparam _INStrm number of input  stream. The advice value is 16 or less.
 * @tparam _ONstrm number of output stream. Should be equal or less than
 * _INStrm.
 * @tparam _TIn input type.
 *
 * @param order_cfg the new order within the window. Each 8bit specifies a new
 * order, starting from the stream with old order 0.
 * @param istrms input data streams.
 * @param e_istrm end flags for input.
 * @param ostrms output data streams.
 * @param e_ostrm end flag for output.
 */
template <int _INStrm, int _ONstrm, typename _TIn>
void stream_shuffle(hls::stream<ap_uint<8 * _INStrm> >& order_cfg,

                    hls::stream<_TIn> istrms[_INStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<_TIn> ostrms[_ONstrm],
                    hls::stream<bool>& e_ostrm);

} // utils_hw
} // common
} // xf

// Implementation =============================================================
namespace xf {
namespace common {
namespace utils_hw {

template <int _INStrm, int _ONstrm, typename _TIn>
void stream_shuffle(hls::stream<ap_uint<8 * _INStrm> >& order_cfg,

                    hls::stream<_TIn> istrms[_INStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<_TIn> ostrms[_ONstrm],
                    hls::stream<bool>& e_ostrm) {

  XF_UTILS_HW_STATIC_ASSERT(_ONstrm <= _INStrm,
                            "stream_shuffle cannot have more output than input.");

  XF_UTILS_HW_STATIC_ASSERT(_INStrm <= 128,
                            "stream_shuffle cannot handle more than 128 streams.");

  ap_uint<7> route[_INStrm];
#pragma HLS ARRAY_PARTITION variable = route complete

  _TIn reg_i[_INStrm];
#pragma HLS ARRAY_PARTITION variable = reg_i complete
  _TIn reg_o[_ONstrm + 1];
#pragma HLS ARRAY_PARTITION variable = reg_o complete

  ap_uint<8 * _INStrm> orders = order_cfg.read();
  for (int i = 0; i < _INStrm; i++) {
#pragma HLS UNROLL
    route[i] = orders.range(8 * i + 6, 8 * i);
    route[i] += 1; // -1 to 0
  }

  bool e = e_istrm.read();
  while (!e) {
#pragma HLS PIPELINE II = 1

    for (int i = 0; i < _INStrm; i++) {
#pragma HLS UNROLL
      reg_i[i] = istrms[i].read();
    }
    e = e_istrm.read();

    for (int i = 0; i < _ONstrm; i++) {
#pragma HLS UNROLL
      reg_o[i] = 0;
    }

    for (int i = 0; i < _INStrm; i++) {
#pragma HLS UNROLL
      reg_o[route[i]] = reg_i[i]; // critical path
    }

    for (int i = 0; i < _ONstrm; i++) {
#pragma HLS UNROLL
      ostrms[i].write(reg_o[i + 1]); // shift by 1
    }

    e_ostrm.write(false);
  }
  e_ostrm.write(true);
}

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_STREAM_SHUFFLE_H

/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file stream_to_axi.h
 * @brief Stream_to_axi template function implementation.
 *
 * This file is part of XF Common Utils Library.
 */

#ifndef XF_UTIL_STRM_TO_AXI_H
#define XF_UTIL_STRM_TO_AXI_H

//#include "xf_util/enums.h"
//#include "xf_util/types.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

/// @brief the template of stream to AXI master port in burst.

/// @tparam WAxi   width of axi port.
/// @tparam WStrm  width of input stream.
/// @tparam NBurst length of a burst.

/// @param wbuf    output AXI port.
/// @param istrm   input stream.
/// @param e_istrm end flag for input stream
template <int WAxi, int WStrm, int NBurst = 16>
void stream_to_axi(ap_uint<WAxi> *wbuf, hls::stream<ap_uint<WStrm>> &istrm,
                   hls::stream<bool> &e_istrm) {
  const int N = WAxi / WStrm;
  ap_uint<WAxi> *pbuf = wbuf;
  ap_uint<WAxi> tmp;
  bool isLast = false;

doing_loop:
  for (int i = 0; i < NBurst * N; i++) {
#pragma HLS PIPELINE II = 1
    int bs = i % N;
    int begin = bs * WStrm;
    isLast = e_istrm.read();
    if (!isLast) {
      ap_uint<WStrm> t = istrm.read();
      tmp(begin + WStrm - 1, begin) = t(WStrm - 1, 0);
    } else
      tmp(begin + WStrm - 1, begin) = 0;
    if (bs == N - 1) {
      pbuf[i / N] = tmp;
    }
  }
#ifndef __SYNTHESIS__
  std::cout << "the hls data is:" << std::endl;
check_value_loop:
  for (int i = 0; i < NBurst; i++) {
    std::cout << "wbuf[" << i << "]=" << pbuf[i] << std::endl;
  }
#endif
}

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_STRM_TO_AXI_H

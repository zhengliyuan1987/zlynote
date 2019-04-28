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
 * @file burst_to_axi.h
 * @brief Stream_to_axi template function implementation.
 *
 * This file is part of XF Common Utils Library.
 */

namespace xf {
namespace util {
namespace level1 {
namespace details {

/// @brief the template of convert stream width from WStrm to WAxi and count
/// burst number.

/// @tparam WAxi   width of axi port.
/// @tparam WStrm  width of input stream.
/// @tparam NBurst length of a burst.

/// @param istrm   input stream.
/// @param e_istrm  end flag for input stream
/// @param axi_strm stream width is WAxi
/// @param nb_strm  store burst number of each burst
template <int WAxi, int WStrm, int NBurst>
void countForBurst(hls::stream<ap_uint<WStrm> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WAxi> >& axi_strm,
                   hls::stream<ap_uint<8> >& nb_strm) {
  const int N = WAxi / WStrm;
  ap_uint<WAxi> tmp;
  bool isLast;
  int nb = 0;
  int bs = 0;

  isLast = e_istrm.read();
doing_loop:
  while (!isLast) {
#pragma HLS pipeline II = 1
    isLast = e_istrm.read();
    int offset = bs * WStrm;
    ap_uint<WStrm> t = istrm.read();
    tmp.range(offset + WStrm - 1, offset) = t(WStrm - 1, 0);
    if (bs == (N - 1)) {
      axi_strm.write(tmp);
      if (nb == (NBurst - 1)) {
        nb_strm.write(NBurst);
        nb = 0;
      } else
        ++nb;
      bs = 0;
    } else
      ++bs;
  }
  // not enough one axi
  if (bs != 0) {
  doing_not_enough:
    for (; bs < N; ++bs) {
#pragma HLS unroll
      int offset = bs * WStrm;
      tmp.range(offset + WStrm - 1, offset) = 0;
    }
    axi_strm.write(tmp);
    ++nb;
  }
  if (nb != 0) {
#ifndef __SYNTHESIS__
    assert(nb <= NBurst);
#endif
    nb_strm.write(nb);
  } else
    nb_strm.write(0);
}

/// @brief the template of stream width of WAxi burst out.

/// @tparam WAxi   width of axi port.
/// @tparam WStrm  width of input stream.
/// @tparam NBurst length of a burst.

/// @tparam WAxi   width of axi port
/// @param axi_strm stream width is WAxi
/// @param nb_strm  store burst number of each burst
template <int WAxi, int WStrm, int NBurst>
void burstWrite(ap_uint<WAxi>* wbuf,
                hls::stream<ap_uint<WAxi> >& axi_strm,
                hls::stream<ap_uint<8> >& nb_strm) {
  // write each burst to axi
  int total = 0;
  ap_uint<WAxi> tmp;
  int n = nb_strm.read();
doing_burst:
  while (n) {
  doing_one_burst:
    for (int i = 0; i < n; i++) {
#pragma HLS pipeline II = 1
      tmp = axi_strm.read();
      wbuf[total * NBurst + i] = tmp;
    }
    total++;
    n = nb_strm.read();
  }
}
} // details

template <int WAxi, int WStrm, int NBurst>
void stream_to_axi(ap_uint<WAxi>* wbuf,
                   hls::stream<ap_uint<WStrm> >& istrm,
                   hls::stream<bool>& e_istrm) {
#ifndef __SYNTHESIS__
  assert(WAxi % WStrm == 0);
#endif
  const int fifo_buf = 2 * NBurst;
  hls::stream<ap_uint<WAxi> > axi_strm;
  hls::stream<ap_uint<8> > nb_strm;
#pragma HLS stream variable = nb_strm depth = 2
#pragma HLS stream variable = axi_strm depth = fifo_buf
#pragma HLS dataflow
  details::countForBurst<WAxi, WStrm, NBurst>(
      istrm, e_istrm, axi_strm, nb_strm);
  details::burstWrite<WAxi, WStrm, NBurst>(wbuf, axi_strm, nb_strm);
}

} // level1
} // util
} // xf

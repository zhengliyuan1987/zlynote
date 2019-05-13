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
 * @file axi_to_multi_stream.h
 * @brief This file is a template implement of loading data from AXI master to multi stream.
 * Xilinx.
 *
 * This file is part of XF Hardware Utilities Library.
 */

#ifndef XF_UTILS_HW_AXI_TO_MULTI_STRM_H
#define XF_UTILS_HW_AXI_TO_MULTI_STRM_H

#include "xf_utils_hw/types.h"
#include "xf_utils_hw/enums.h"

// Forward decl

namespace xf {
namespace common {
namespace utils_hw {
namespace details {

// ---------------------- details ---------------------------------
template <int _WAxi, int _BurstLen>
void axi_onetype_batch_to_ram(const ap_uint<_WAxi>* rbuf,
                              const int off_axi,
                              const int len,
                              int& pos,
                              ap_uint<_WAxi> dat_ram[_BurstLen],
                              int& len_ram,
                              int& pos_ram) {
  if (pos == len) return;
  if (len_ram != pos_ram)
    return;
  else {
    // aligned to read len times
    const ap_uint<_WAxi>* vec_ptr = rbuf + pos + off_axi;

    for (int n = 0; n < _BurstLen; n++) {
#pragma HLS loop_tripcount min = 1 max = 1
#pragma HLS PIPELINE II = 1
      dat_ram[n] = vec_ptr[n];
    }
    if (_BurstLen >= len - pos) { // All data has been copied
      len_ram = len - pos;
      pos = len;
    } else { // Full size is copied but still remain one or more
      len_ram = _BurstLen;
      pos += _BurstLen;
    }
    pos_ram = 0;
  }
}

template <int _WAxi, int _BurstLen>
bool non_blocking_onetype_ram_to_stream(
    bool is_onetype_fnl,
    const int len,
    const int pos,
    ap_uint<_WAxi> dat_ram[_BurstLen],
    const int len_ram,
    int& pos_ram,
    hls::stream<ap_uint<_WAxi> >& vec_strm) {
  if (is_onetype_fnl) {
    return is_onetype_fnl;
  }
  bool isFinalBlock = len == pos;
  bool hasDone_blk = len_ram == pos_ram;
  bool isFull = false;
  ap_uint<_WAxi> vec_reg;

NON_BLOCKING_LOOP:
  while (isFull == false && hasDone_blk == false) {
#pragma HLS loop_tripcount min = 1 max = 32 avg = 32
#pragma HLS PIPELINE II = 1
    ap_uint<_WAxi> rd = 0;
    isFull = vec_strm.full();
    rd = dat_ram[pos_ram];

    is_onetype_fnl = (!isFull) & (isFinalBlock) & (((pos_ram + 1) == len_ram))
                         ? true
                         : false;
    hasDone_blk = (!isFull) & (((pos_ram + 1) == len_ram)) ? true : false;
    if (!isFull) {
      vec_strm.write(rd);
#ifndef __SYNTHESIS__
//            cnt_test++;
#endif
    }
    (pos_ram)++;
  } // end while

  if (isFull) {
    (pos_ram)--;
  }
  return is_onetype_fnl;
}

template <int _WAxi, int _BurstLen>
bool axi_batchdata_to_stream(const ap_uint<_WAxi>* rbuf,
                             const int off_axi,
                             const int len,
                             int& pos,
                             ap_uint<_WAxi> dat_ram[_BurstLen],
                             int& len_ram,
                             int& pos_ram,
                             bool is_onetype_fnl,
                             hls::stream<ap_uint<_WAxi> >& vec_strm) {
  /////////////// load data to local ram  ///////////////
  details::axi_onetype_batch_to_ram<_WAxi, _BurstLen>(
      rbuf, off_axi, len, pos, dat_ram, len_ram, pos_ram);

  /////////////// nonblocking write one type data form ram to strm
  //////////////////
  bool is_fnl = details::non_blocking_onetype_ram_to_stream<_WAxi, _BurstLen>(
      is_onetype_fnl, len, pos, dat_ram, len_ram, pos_ram, vec_strm);
  return is_fnl;
}

template <int _WAxi, int _BurstLen, int _NT>
void read_to_vec_stream(ap_uint<_WAxi>* rbuf,
                        hls::stream<ap_uint<_WAxi> > vec_strm[_NT],
                        const int len[_NT],
                        const int offset[_NT]) {
  const int scal_char = _WAxi / 8;
  int len_vec[_NT]; // length for one type vector, by width of Axi
  int pos_vec[_NT]; // current position for one type vec, by width of Axi
  int len_ram[_NT]; // length for one type vector loaded in local ram
  int pos_ram[_NT]; // current position need to be read
  int off_ali[_NT]; // the first row offset aligned to Axi port by char
  int off_axi[_NT]; // offset aligned by axi width
  ap_uint<_WAxi> dat_ram[_NT]
                        [_BurstLen]; // local ram depth equals the burst length
  int cnt_alltype_fnl;
  bool is_onetype_fnl[3];
#pragma HLS RESOURCE variable = dat_ram core = RAM_2P_BRAM

#pragma HLS ARRAY_PARTITION variable = vec_strm complete

#pragma HLS ARRAY_PARTITION variable = len complete
#pragma HLS ARRAY_PARTITION variable = offset complete
#pragma HLS ARRAY_PARTITION variable = off_ali complete
#pragma HLS ARRAY_PARTITION variable = len_vec complete
#pragma HLS ARRAY_PARTITION variable = pos_vec complete
#pragma HLS ARRAY_PARTITION variable = len_ram complete
#pragma HLS ARRAY_PARTITION variable = off_axi complete
#pragma HLS ARRAY_PARTITION variable = dat_ram complete dim = 1
#pragma HLS ARRAY_PARTITION variable = is_onetype_fnl complete
/////////////// init ///////////////
INIT0:
  for (int t = 0; t < _NT; t++) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = 1
#pragma HLS PIPELINE II = 1
    off_ali[t] = (offset[t]) & (scal_char - 1); // scal_char is always 2^N
  }
INIT1:
  for (int t = 0; t < _NT; t++) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = 1
#pragma HLS PIPELINE II = 1
    len_vec[t] = (len[t] + off_ali[t] + scal_char - 1) / scal_char;
    off_axi[t] =
        (offset[t] + scal_char - 1) / scal_char - ((off_ali[t]) ? 1 : 0);
    pos_vec[t] = 0;
    len_ram[t] = 0;
    pos_ram[t] = 0;
    if (t < _NT)
      is_onetype_fnl[t] = (len[t] == 0);
    else
      is_onetype_fnl[t] = true;
  }

/////////////// round robin to write ///////////////
ROUND_ROBIN:
  do {
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min = 1 max = 1
    cnt_alltype_fnl = 0;

    for (int t = 0; t < _NT; t++) {
#pragma HLS UNROLL
      is_onetype_fnl[t] =
          details::axi_batchdata_to_stream<_WAxi, _BurstLen>(rbuf,
                                                           off_axi[t],
                                                           len_vec[t],
                                                           pos_vec[t],
                                                           dat_ram[t],
                                                           len_ram[t],
                                                           pos_ram[t],
                                                           is_onetype_fnl[t],
                                                           vec_strm[t]);
      cnt_alltype_fnl += (is_onetype_fnl[t] == true);
    }

  } while (cnt_alltype_fnl != _NT);
}

template <int _WAxi, typename _TStrm, int scal_vec>
void split_vec_to_aligned_duplicate(hls::stream<ap_uint<_WAxi> >& vec_strm,
                                    const int len,
                                    const int scal_char,
                                    const int offset,
                                    hls::stream<_TStrm>& r_strm,
                                    hls::stream<bool>& e_strm) {
  //    if(len== 0)//explanatory note for len == 0 case
  //        return ;

  const int nread = (len + offset + scal_char - 1) / scal_char;
  // n read times except the first read, n_read+1 = total read times
  int cnt_r = nread - 1;
  const int nwrite = (len + sizeof(_TStrm) - 1) / sizeof(_TStrm);
  const int WStrm = 8 * sizeof(_TStrm);
  // first read is specific
  ap_uint<_WAxi> vec_reg = vec_strm.read();
  ap_uint<_WAxi> vec_aligned = 0;

  if (offset) {
  LOOP_SPLIT_VEC_TO_ALIGNED:
    for (int i = 0; i < nwrite; i += scal_vec) {
#pragma HLS loop_tripcount min = 1 max = 1
#pragma HLS PIPELINE II = scal_vec
      vec_aligned((scal_char - offset << 3) - 1, 0) =
          vec_reg((scal_char << 3) - 1, offset << 3);
      if ((scal_char - offset) < len && (cnt_r != 0)) { // always need read
                                                        // again
        ap_uint<_WAxi> vec = vec_strm.read();
        vec_aligned((scal_char << 3) - 1, (scal_char - offset) << 3) =
            vec(offset << 3, 0);
        vec_reg((scal_char << 3) - 1, offset << 3) =
            vec((scal_char << 3) - 1, offset << 3);
        cnt_r--;
      } // else few cases no read again
      int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;
      for (int j = 0; j < scal_vec; ++j) {
#pragma HLS PIPELINE II = 1
        ap_uint<WStrm> r0 = vec_aligned.range(WStrm * (j + 1) - 1, WStrm * j);
        if (j < n) {
          r_strm.write((_TStrm)r0);
          e_strm.write(false);
        } // end if
      }
    } // end loop
  }   // end if

  if (!offset) {
  // no read
  SPLIT_VEC:
    int fst_n = scal_vec > nwrite ? nwrite : scal_vec;
    for (int j = 0; j < scal_vec; ++j) {
#pragma HLS PIPELINE II = 1
      ap_uint<WStrm> r0 = vec_reg.range(WStrm * (j + 1) - 1, WStrm * j);
      if (j < fst_n) {
        r_strm.write((_TStrm)r0);
        e_strm.write(false);
      }
    }

    for (int i = scal_vec; i < nwrite; i += scal_vec) {
#pragma HLS loop_tripcount min = 1 max = 1
#pragma HLS PIPELINE II = scal_vec
      ap_uint<_WAxi> vec = vec_strm.read();
      int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;

      for (int j = 0; j < scal_vec; ++j) {
#pragma HLS PIPELINE II = 1
        ap_uint<WStrm> r0 = vec.range(WStrm * (j + 1) - 1, WStrm * j);
        if (j < n) {
          r_strm.write((_TStrm)r0);
          e_strm.write(false);
        }
      }
    }
  } // end if

  e_strm.write(true);
}

} // details

// ---------------------- APIs ---------------------------------

/**
 * @brief Loading multiple categories of data from one AXI master to streams.
 *
 * This primitive assumes the width of AXI port is multiple of alignment width.
 * When alignment width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * AXI port width and width of each type are assumed to be multiple of 8.
 * It is assumed that the data width in bits is ``8 * sizeof(T)``, and data
 * type can be casted from raw bits of matching width.
 *
 * This module assumes the data is tightly packed, so that the begining of
 * Type 2 data may be placed in one AXI port row with the end of Type 1 data.
 *
 * \rst
 * ::
 *
 *     AXI word [ elements of Type 1 ........................................ ]
 *     AXI word [ elements of Type 1 ..... end | begin elements of Type 2 ... ]
 *     AXI word [ elements of Type 2 ........................................ ]
 *
 * \endrst
 *
 * @tparam _BurstLen burst length.
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _TStrm0 first stream's type.
 * @tparam _TStrm1 second stream's type.
 * @tparam _TStrm2 third stream's type.
 *
 * @param rbuf input AXI port.
 * @param ostrm0 output stream of type 0.
 * @param e_ostrm0 end flag for output stream of type 0.
 * @param ostrm1 output stream of type 1.
 * @param e_ostrm1 end flag for output stream of type 1.
 * @param ostrm2 output stream of type 2.
 * @param e_ostrm2 end flag for output stream of type 2.
 * @param num number of elements to load from AXI port for each type.
 * @param offset offset for each type, in number of chars.
 */
template <int _BurstLen,
          int _WAxi,
          typename _TStrm0,
          typename _TStrm1,
          typename _TStrm2>
void axi_to_multi_stream(ap_uint<_WAxi>* rbuf,
                         hls::stream<_TStrm0>& ostrm0,
                         hls::stream<bool>& e_ostrm0,
                         hls::stream<_TStrm1>& ostrm1,
                         hls::stream<bool>& e_ostrm1,
                         hls::stream<_TStrm2>& ostrm2,
                         hls::stream<bool>& e_ostrm2,
                         const int num[3],
                         const int offset[3]) {
#pragma HLS DATAFLOW

// The depth of FIFO in the circuit which is only related to the ability to
// buffer data, but not to the speed of the circuit. when NONBLOCK_DEPTH > 1024,
// usage of bram will increase
const int NONBLOCK_DEPTH = (256);

  hls::stream<ap_uint<_WAxi> > vec_strm[3];
#pragma HLS RESOURCE variable = vec_strm core = FIFO_LUTRAM
#pragma HLS STREAM variable = vec_strm depth = NONBLOCK_DEPTH
#pragma HLS ARRAY_PARTITION variable = vec_strm complete
#pragma HLS ARRAY_PARTITION variable = num complete
#pragma HLS ARRAY_PARTITION variable = offset complete

  const int scal_char = _WAxi / 8;
  const int scal_vec0 = _WAxi / (8 * sizeof(_TStrm0));
  const int scal_vec1 = _WAxi / (8 * sizeof(_TStrm1));
  const int scal_vec2 = _WAxi / (8 * sizeof(_TStrm2));

  // Copy parameter to local
  int off_ali[3];
#pragma HLS ARRAY_PARTITION variable = off_ali complete
  for (int t = 0; t < 3; t++) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = 1
#pragma HLS PIPELINE II = 1
    off_ali[t] = (offset[t]) & (scal_char - 1); // scal_char is always 2^N
  }

  details::read_to_vec_stream<_WAxi, _BurstLen, 3>(rbuf, vec_strm, num, offset);
  details::split_vec_to_aligned_duplicate<_WAxi, _TStrm0, scal_vec0>(
      vec_strm[0], num[0], scal_char, off_ali[0], ostrm0, e_ostrm0);
  details::split_vec_to_aligned_duplicate<_WAxi, _TStrm1, scal_vec1>(
      vec_strm[1], num[1], scal_char, off_ali[1], ostrm1, e_ostrm1);
  details::split_vec_to_aligned_duplicate<_WAxi, _TStrm2, scal_vec2>(
      vec_strm[2], num[2], scal_char, off_ali[2], ostrm2, e_ostrm2);
}

// TODO for 2 and for 4

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_AXI_TO_MULTI_STRM_H

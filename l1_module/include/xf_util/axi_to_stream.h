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
 * @file axi_to_stream.h
 * @brief This file is a template implement of loading data from AXI master to stream.
 * Xilinx.
 *
 * This file is part of XF UTIL LIBRARY.
 */

#ifndef XF_UTIL_AXI_TO_STREAM_H
#define XF_UTIL_AXI_TO_STREAM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"


#ifndef __SYNTHESIS__
#include <cstdio>// to be move to the common file of this library
#endif

namespace xf {
namespace common {
namespace utils_hw {
namespace details {

// ---------------------- burst_read_axi_alignedData_to_vec ---------------------------------
template <int _WAxi, int _BstLen>
void read_to_vec(
    ap_uint<_WAxi>* vec_ptr,
    const int nrow,
    const int scal_vec,
    hls::stream<ap_uint<_WAxi> >& vec_strm
){

    const int nread = (nrow + scal_vec - 1) / scal_vec;

READ_TO_VEC:
    for (int i = 0; i < nread; i += _BstLen) {
#pragma HLS loop_tripcount min=1 max=1
//#pragma HLS PIPELINE II = _BstLen
    int len = ((i + _BstLen) > nread) ? (nread - i) : _BstLen;
    READ_VEC0:
        for (int j = 0; j < len; ++j) {
#pragma HLS PIPELINE II = 1
            vec_strm.write(vec_ptr[i + j]);
        }//This pipeline must be no judgment, otherwise the tool will not be able to derive the correct burst_len

    }
}

template <int _WAxi, typename _TStrm, int scal_vec >
void split_vec(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int nrow,
    const int offset_AL,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm) {

    const int WStrm = 8*sizeof(_TStrm);
    ap_uint<_WAxi> fst_vec = vec_strm.read();
    int fst_n = (scal_vec-offset_AL) > nrow ? (nrow+offset_AL)  : scal_vec;

SPLIT_FEW_VEC:
    for (int j = 0; j < scal_vec; ++j) {
#pragma HLS loop_tripcount min=1 max=1
#pragma HLS PIPELINE II = 1
        ap_uint<WStrm > fst_r0 =
        fst_vec.range(WStrm * (j + 1) - 1, WStrm * j);
        if (j < fst_n && j >= offset_AL) {
            r_strm.write((_TStrm)fst_r0);
            e_strm.write(false);
        }
    }

SPLIT_VEC:
    for (int i = scal_vec - offset_AL; i < nrow; i += scal_vec) {
#pragma HLS loop_tripcount min=1 max=1
#pragma HLS PIPELINE II = scal_vec
        ap_uint<_WAxi> vec = vec_strm.read();
        int n = (i + scal_vec) > nrow ? (nrow - i) : scal_vec;

        for (int j = 0; j < scal_vec; ++j) {
#pragma HLS PIPELINE II = 1
            ap_uint<WStrm > r0 =
            vec.range(WStrm * (j + 1) - 1, WStrm * j);
            if (j < n) {
                r_strm.write((_TStrm)r0);
                e_strm.write(false);
            }
        }

     }
     e_strm.write(true);
}

// ---------------------- burst_read_axi_GeneralData_to_fixedLengthVector_stream ---------------------------------

template <int _WAxi, int _BstLen>
void read_to_vec(
    ap_uint<_WAxi>* vec_ptr,
    const int len,
    const int scal_char,
    const int offset,
    hls::stream<ap_uint<_WAxi> >& vec_strm) {

    const int nread = (len + offset + scal_char - 1) / scal_char;

READ_TO_VEC:
    for (int i = 0; i < nread; i += _BstLen) {
#pragma HLS loop_tripcount min=1 max=1
//#pragma HLS PIPELINE II = _BstLen
        int len = ((i + _BstLen) > nread) ? (nread - i) : _BstLen;

    READ_VEC0:
    for (int j = 0; j < len; ++j) {
#pragma HLS PIPELINE II = 1
            vec_strm.write(vec_ptr[i + j]);
        }//This pipeline must be no judgment, otherwise the tool will not be able to derive the correct burst_len
    }
}

template <int _WAxi, typename _TStrm, int scal_vec >
void split_vec_to_aligned(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int len,
    const int scal_char,
    const int offset,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm
){

	const int nread = (len + offset+ scal_char - 1) / scal_char ;
	//n read times except the first read, n_read+1 = total read times
	      int cnt_r = nread -1;
	const int nwrite = (len + sizeof(_TStrm) - 1) / sizeof(_TStrm);
    const int WStrm = 8*sizeof(_TStrm);
    //first read is specific
    ap_uint<_WAxi> vec_reg = vec_strm.read();
    ap_uint<_WAxi> vec_aligned = 0;

    if( offset ){

        LOOP_SPLIT_VEC_TO_ALIGNED:
        for (int i = 0; i < nwrite; i += scal_vec) {
        #pragma HLS loop_tripcount min=1 max=1
        #pragma HLS PIPELINE II = scal_vec
              vec_aligned((scal_char-offset<<3)-1, 0)              = vec_reg((scal_char<<3)-1, offset<<3);
        	  if((scal_char-offset)<len&&(cnt_r!=0)){//always need read again
                  ap_uint<_WAxi> vec = vec_strm.read();
                  vec_aligned((scal_char<<3)-1, (scal_char-offset)<<3) = vec(offset<<3, 0);
                  vec_reg    ((scal_char<<3)-1, offset<<3)             = vec((scal_char<<3)-1, offset<<3);
                  cnt_r--;
        	  }//else few cases no read again
        	  int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;
              for (int j = 0; j < scal_vec; ++j) {
        #pragma HLS PIPELINE II = 1
                  ap_uint<WStrm > r0 =
                      vec_aligned.range(WStrm * (j + 1) - 1, WStrm*j);
                  if (j < n) {
                      r_strm.write((_TStrm)r0);
                      e_strm.write(false);
                  }//end if
              }
          }//end loop
    }

    if( !offset ){
    	//no read
    	SPLIT_VEC:
		int fst_n =  scal_vec > nwrite ? nwrite  : scal_vec;
		for (int j = 0; j < scal_vec; ++j) {
		#pragma HLS PIPELINE II = 1
			ap_uint<WStrm > r0 =
			vec_reg.range(WStrm * (j + 1) - 1, WStrm * j);
			if (j < fst_n) {
				r_strm.write((_TStrm)r0);
				e_strm.write(false);
			}
		}

		for (int i = scal_vec; i < nwrite; i += scal_vec) {
		#pragma HLS loop_tripcount min=1 max=1
		#pragma HLS PIPELINE II = scal_vec
			ap_uint<_WAxi> vec = vec_strm.read();
			int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;

			for (int j = 0; j < scal_vec; ++j) {
		#pragma HLS PIPELINE II = 1
				ap_uint<WStrm > r0 =
				vec.range(WStrm * (j + 1) - 1, WStrm * j);
				if (j < n) {
					r_strm.write((_TStrm)r0);
					e_strm.write(false);
				}
			}
		 }
    }
    e_strm.write(true);
}

} // details

// ---------------------- APIs ---------------------------------

/* @brief Loading data from AXI master to fixed_width stream.
 * This primitive is relatively universal compared with AXI port of aligned data primitives.
 *
 * AXI port is assumed to have width as multiple of 8-bit char.
 * The data width cloud be unaligned or aligned, e.g. compressed binary files.
 *********************
 * DDR   ->  AXI_BUS                          ->  FIFO  ->   strm
 * XXX1     XXX1234567323334_3536373839646566    XXX12345    1234
 * ...      ...                                  67323334    5673
 *                                               ...         ...
 * 32XX     8123456732XXXXXX_XXXXXXXXXXXXXXXX    32XXXXXX    32XX
 *********************
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _BstLen burst length of the AXI port
 * @tparam _TStrm stream's type, e.g. ap_uint<fixed_width> for a fixed_width stream.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param len number of char to load from AXI port.
 * @param offset offset from the beginning of the buffer, in number of char.
 */
template <int _WAxi, int _BstLen, typename _TStrm>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<_TStrm >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int len,
    const int offset = 0
){

#pragma HLS DATAFLOW
      const int fifo_depth = _BstLen * 2;
      const int size0      = sizeof(_TStrm);
      const int scal_vec   = _WAxi/(8*size0);
      const int scal_char  = _WAxi/8;

      hls::stream<ap_uint<_WAxi> > vec_strm;
#pragma HLS RESOURCE variable= vec_strm core  = FIFO_LUTRAM
#pragma HLS STREAM  variable = vec_strm depth = fifo_depth

      details::read_to_vec<_WAxi, _BstLen >(
          rbuf, len, scal_char, offset,
          vec_strm);

	  details::split_vec_to_aligned<_WAxi, _TStrm , scal_vec>(
		  vec_strm, len, scal_char, offset,
		  ostrm, e_ostrm);

}

/* @brief Loading data from AXI master to aligned_width stream.
 * Lightweight primitives for aligned data.
 *
 * This primitive assumes the data in DDR is aligned.
 * which means the width of AXI port is positive integer multiple of alignment width
 * and the stream's width just equals the aligned width.
 * When input data ptr width is less than AXI port width, the AXI port bandwidth
 * will not be fully used. So, AXI port width should be minimized while meeting
 * performance requirements of application.
 *
 * Both AXI port and alignment width are assumed to be multiple of 8-bit char.
 * The data width must be no greater than its alignment width.
 *********************
 *DDR->  AXI_BUS   ->  FIFO  ->     strm(aligned to dd)
 *
 *XXaa   XXaabbcc    XXaabbcc       aa
 *bbcc   ddXX0000    ddXX0000       ...
 *ddXX                              dd
 *********************
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _BstLen burst length of AXI buffer.
 * @tparam _TStrm stream's type, e.g. ap_uint<aligned_width> for a aligned_width stream.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
 * @param offset_AL offset from the beginning of the buffer, by aligned width width.
 */
template <int _WAxi, int _BstLen,  typename _TStrm>
void axi_to_stream(
    ap_uint<_WAxi>*         rbuf,
    const int               num,
    hls::stream<_TStrm >&   ostrm,
    hls::stream<bool>&      e_ostrm,
    const int               offset_AL = 0
){
#pragma HLS DATAFLOW
      const int fifo_depth = _BstLen * 2;
      const int size0      = sizeof(_TStrm);
      const int scal_vec   = _WAxi/(8*size0);
      const int scal_char  = _WAxi/8;

      hls::stream<ap_uint<_WAxi> > vec_strm;
#pragma HLS RESOURCE variable= vec_strm core  = FIFO_LUTRAM
#pragma HLS STREAM  variable = vec_strm depth = fifo_depth

      details::read_to_vec<_WAxi, _BstLen>(
          rbuf, num, scal_vec,
          vec_strm);

      details::split_vec<_WAxi, _TStrm, scal_vec >(
          vec_strm, num, offset_AL,
          ostrm, e_ostrm);
}

} // utils_hw
} // common
} // xf


#endif // XF_UTIL_AXI_TO_STRM_H

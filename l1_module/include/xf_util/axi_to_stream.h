

#ifndef XF_UTIL_AXI_TO_STREAM_H
#define XF_UTIL_AXI_TO_STREAM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"


#ifndef __SYNTHESIS__
#include <cstdio>// to be move to the common file of this library
#endif

namespace xf {
namespace util {
namespace level1 {
namespace details{
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
#pragma HLS PIPELINE II = _BstLen
//#pragma HLS DATAFLOW
    int len = ((i + _BstLen) > nread) ? (nread - i) : _BstLen;
    READ_VEC0:
        for (int j = 0; j < len; ++j) {
#pragma HLS PIPELINE II = 1
            vec_strm.write(vec_ptr[i + j]);
        }

    }
}

template <int _WAxi, typename _TStrm, int scal_vec>
void split_vec(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int nrow,
	const int offset_AL,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm) {

	ap_uint<_WAxi> fst_vec = vec_strm.read();
	int fst_n = (scal_vec-offset_AL) > nrow ? (nrow+offset_AL)  : scal_vec;
	for (int j = 0; j < scal_vec; ++j) {
#pragma HLS PIPELINE II = 1
		ap_uint<8 * sizeof(_TStrm)> fst_r0 =
		fst_vec.range(8 * sizeof(_TStrm) * (j + 1) - 1, 8 * sizeof(_TStrm) * j);
		if (j < fst_n && j >= offset_AL) {
			r_strm.write((_TStrm)fst_r0);
			e_strm.write(false);
		}
	}

SPLIT_VEC:
    for (int i = scal_vec - offset_AL; i < nrow; i += scal_vec) {
#pragma HLS PIPELINE II = scal_vec
        ap_uint<_WAxi> vec = vec_strm.read();
        int n = (i + scal_vec) > nrow ? (nrow - i) : scal_vec;

        for (int j = 0; j < scal_vec; ++j) {
            ap_uint<8 * sizeof(_TStrm)> r0 =
            vec.range(8 * sizeof(_TStrm) * (j + 1) - 1, 8 * sizeof(_TStrm) * j);
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
#pragma HLS PIPELINE II = _BstLen
        int len = ((i + _BstLen) > nread) ? (nread - i) : _BstLen;

    READ_VEC0:
        for (int j = 0; j < len; ++j) {
#pragma HLS PIPELINE II = 1
            vec_strm.write(vec_ptr[i + j]);
        }
    }
}

template <int _WAxi,  typename _TStrm, int scal_vec>
void split_vec_to_aligned(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int len,
	const int scal_char,
	const int offset,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm
){

	//const int size_strm = _WStrm/8;
	const int nwrite = (len + sizeof(_TStrm) - 1) / sizeof(_TStrm);
	ap_uint<_WAxi> vec_reg = vec_strm.read();
	ap_uint<_WAxi> vec_aligned = 0;

	if((scal_char-offset)<len){

		//need read again
		LOOP_SPLIT_VEC_TO_ALIGNED:
		  for (int i = 0; i < nwrite; i += scal_vec) {
		#pragma HLS PIPELINE II = scal_vec
			  ap_uint<_WAxi> vec = vec_strm.read();
			  vec_aligned((scal_char-offset<<3)-1, 0) 			   = vec_reg((scal_char<<3)-1, offset<<3);
			  vec_aligned((scal_char<<3)-1, (scal_char-offset)<<3) = vec(offset<<3, 0);
			  vec_reg    ((scal_char<<3)-1, offset<<3)             = vec((scal_char<<3)-1, offset<<3);
		      int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;
		      for (int j = 0; j < scal_vec; ++j) {
		          ap_uint<8*sizeof(_TStrm) > r0 =
		        	  vec_aligned.range(8*sizeof(_TStrm) * (j + 1) - 1, 8*sizeof(_TStrm)*j);
				  if (j < n) {
					  r_strm.write((_TStrm)r0);
					  e_strm.write(false);
				  }//end if
		      }
		  }

	}else{

		//no read
		SPLIT_VEC_TO_ALIGNED:
		vec_aligned((scal_char-offset)<<3-1, 0) = vec_reg((scal_char<<3)-1, offset<<3);
	    for (int j = 0; j < scal_vec; ++j) {
		#pragma HLS PIPELINE II = 1
	        ap_uint<8*sizeof(_TStrm) > r0 =
	    	    vec_aligned.range(8*sizeof(_TStrm) * (j + 1) - 1, 8*sizeof(_TStrm)*j);
	        if (j < nwrite) {
	            r_strm.write((_TStrm)r0);
	            e_strm.write(false);
	        }//end if
	    }

	}
    e_strm.write(true);
}

template <int _WAxi, typename _TStrm, int scal_vec>
void split_vec(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int len,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm) {

	const int nwrite = (len + sizeof(_TStrm) - 1) / sizeof(_TStrm);

SPLIT_VEC:
    for (int i = 0; i < nwrite; i += scal_vec) {
#pragma HLS PIPELINE II = scal_vec
        ap_uint<_WAxi> vec = vec_strm.read();
        int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;

        for (int j = 0; j < scal_vec; ++j) {
            ap_uint<8 * sizeof(_TStrm)> r0 =
            vec.range(8 * sizeof(_TStrm) * (j + 1) - 1, 8 * sizeof(_TStrm) * j);
            if (j < n) {
                r_strm.write((_TStrm)r0);
                e_strm.write(false);
            }
        }

     }
     e_strm.write(true);
}

} // details



/* @brief Loading data from AXI master to stream.
 *
 * This primitive assumes the width of AXI port is multiple of data width.
 * Data width could be unaligned.
 * When input data ptr width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * Both AXI port and data are assumed to have width as multiple of 8-bit char.
 * The data width is unaligned, e.g. compressed binary files.
 *********************
 *DDR   ->  AXI_BUS                          ->  FIFO  ->  	strm
 *XXX1     XXX1234567323334_3536373839646566    XXX12345    1234
 *...      ...									67323334    5673
 *      									    ...			...
 *32XX     8123456732XXXXXX_XXXXXXXXXXXXXXXX    32XXXXXX    32XX
 *********************
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _WStrm width of the stream.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
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

	  details::read_to_vec<_WAxi, _BstLen>(
		  rbuf, len, scal_char, offset,
	      vec_strm);
	  if(!offset){
		  details::split_vec<_WAxi, _TStrm, scal_vec>(
		      vec_strm, len,
			  ostrm, e_ostrm);
	  }else{
		  details::split_vec_to_aligned<_WAxi, _TStrm, scal_vec>(
			  vec_strm, len, scal_char, offset,
			  ostrm, e_ostrm);
	  }
}

/* @brief Loading data from AXI master to stream.
 *
 * This primitive assumes the width of AXI port is positive integer multiple of alignment width.
 * When input data ptr width is less than AXI port width, the AXI port bandwidth
 * will not be fully used. So, AXI port width should be minimized while meeting
 * performance requirements of application.
 *
 * Both AXI port and alignment width are assumed to be multiple of 8.
 * The data width must be no greater than its alignment width.
 *********************
 *DDR->  AXI_BUS   ->  FIFO  ->  	strm(aligned to dd)
 *
 *XXaa   XXaabbcc    XXaabbcc		aa
 *bbcc   ddXX0000    ddXX0000		...
 *ddXX				   				dd
 *********************
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _BstLen burst lenth of AXI buffer.
 * @tparam _TStrm data's type.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
 * @param offset_AXI offset from the beginning of the buffer, by AXI port width.
 */
template <int _WAxi, int _BstLen,  typename _TStrm>
void axi_to_stream(
    ap_uint<_WAxi>* 		rbuf,
    const int 				num,
    hls::stream<_TStrm >& 	ostrm,
    hls::stream<bool>& 		e_ostrm,
	const int 				offset_AL = 0
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

	  details::split_vec<_WAxi, _TStrm, scal_vec>(
	      vec_strm, num, offset_AL,
		  ostrm, e_ostrm);
}

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_STRM_H

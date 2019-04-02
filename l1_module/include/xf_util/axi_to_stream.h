

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
    hls::stream<ap_uint<_WAxi> >& vec_strm) {

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
#ifndef __SYNTHESIS__
     printf("%d burst len %d\n", i / _BstLen, len);
#endif
  }
}

template <int _WAxi, typename _TStrm, int scal_vec>
void split_vec(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int nrow,
	const int size0,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm) {

SPLIT_VEC:
  for (int i = 0; i < nrow; i += scal_vec) {
#pragma HLS PIPELINE II = scal_vec
    ap_uint<_WAxi> vec = vec_strm.read();
    int n = (i + scal_vec) > nrow ? (nrow - i) : scal_vec;
    for (int j = 0; j < scal_vec; ++j) {
      ap_uint<8 * sizeof(_TStrm)> r0 =
          vec.range(8 * size0 * (j + 1) - 1, 8 * size0 * j);
      if (j < n) {
        r_strm.write((_TStrm)r0);
        e_strm.write(false);
      }
    }
  }
  e_strm.write(true);
}

// ---------------------- burst_read_axi_UnalignedData_to_ram ---------------------------------
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
#ifndef __SYNTHESIS__
     printf("%d burst len %d\n", i / _BstLen, len);
#endif
  }
}

template <int _WAxi, int _WStrm, int scal_vec>
void split_vec_to_aligned(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
	const int num,
    const int len,
	const int scal_char,
	const int offset,
	int nrow,
    hls::stream<ap_uint<_WStrm> >& r_strm,
    hls::stream<bool>& e_strm
){

	const int size_strm = _WStrm/8;
	const int nwrite = (len + size_strm - 1) / size_strm;
	ap_uint<_WAxi> vec_reg = vec_strm.read();
	ap_uint<_WAxi> vec_aligned;

	vec_aligned(scal_char-offset-1, 0) = vec_reg(scal_char-1, offset);

	if((scal_char-offset)<len){

		//need read again
		SPLIT_VEC_TO_ALIGNED:
		  for (int i = 0; i < nwrite; i += scal_vec) {
		#pragma HLS PIPELINE II = scal_vec
			  ap_uint<_WAxi> vec = vec_strm.read();
			  vec_aligned(scal_char-1, scal_char-offset) = vec_reg(offset, 0);
		      int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;
		      for (int j = 0; j < scal_vec; ++j) {
		          ap_uint<_WStrm > r0 =
		        	  vec_aligned.range(_WStrm * (j + 1) - 1, _WStrm*j);
		      if (j < n) {
		        r_strm.write(r0);
		        e_strm.write(false);
		      }//end if
		    }
		  }

	}else{

		//no read
		SPLIT_VEC_TO_ALIGNED:
	    for (int j = 0; j < scal_vec; ++j) {
	        ap_uint<_WStrm > r0 =
	    	    vec_aligned.range(_WStrm * (j + 1) - 1, _WStrm*j);
	        if (j < nwrite) {
	            r_strm.write(r0);
	            e_strm.write(false);
	      }//end if
	    }

	}
  e_strm.write(true);
  nrow = num;
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
 *dxdx     XXX1234567323334_3536373839646566    XXX12345    1234
 *...      ...									67323334    5673
 *      									    ...			...
 *xdxd     8123456732XXXXXX_XXXXXXXXXXXXXXXX    32XXXXXX    32XX
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
template <int _WAxi, int _BstLen, int _WStrm>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_WStrm> >& ostrm,
    hls::stream<bool>& e_ostrm,
	      int nrow,
    const int num,
	const int len,
    const int offset = 0
){

#pragma HLS DATAFLOW
	  const int fifo_depth = _BstLen * 2;
	  const int scal_vec   = _WAxi/_WStrm;
	  const int scal_char  = _WAxi/8;

	  hls::stream<ap_uint<_WAxi> > vec_strm;
#pragma HLS RESOURCE variable= vec_strm core  = FIFO_LUTRAM
#pragma HLS STREAM  variable = vec_strm depth = fifo_depth


	  ap_uint<_WAxi>* 	vec_ptr= rbuf;
	  details::read_to_vec<_WAxi, _BstLen>(
	      vec_ptr, len, scal_char, offset,
	      vec_strm);

	  details::split_vec_to_aligned<_WAxi, _WStrm, scal_vec>(
	      vec_strm, num, len, scal_char, offset,
		  nrow, ostrm, e_ostrm);
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
 *DDR->  AXI_BUS   ->  FIFO  ->  	strm
 *
 *dddd   dddddddd    dddddddd		d
 *dddd   0000dddd    0000dddd		d
 *dddd				   				...
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
 * @param offset offset from the beginning of the buffer, by AXI port width.
 */
template <int _WAxi, int _BstLen,  typename _TStrm>
void axi_to_stream(
    ap_uint<_WAxi>* 		rbuf,
    hls::stream<_TStrm >& 	ostrm,
    hls::stream<bool>& 		e_ostrm,
    const int 				num,
    const int 				offset = 0
){
#pragma HLS DATAFLOW
	  const int fifo_depth = _BstLen * 2;
	  const int size0      = sizeof(_TStrm);
	  const int scal_vec   = _WAxi/(8*size0);
	  const int scal_char  = _WAxi/8;

	  hls::stream<ap_uint<_WAxi> > vec_strm;
#pragma HLS RESOURCE variable= vec_strm core  = FIFO_LUTRAM
#pragma HLS STREAM  variable = vec_strm depth = fifo_depth


	  ap_uint<_WAxi>* 	vec_ptr= rbuf + offset;
	  details::read_to_vec<_WAxi, _BstLen>(
	      vec_ptr, num, scal_vec,
	      vec_strm);

	  details::split_vec<_WAxi, _TStrm, scal_vec>(
	      vec_strm, num, size0,
		  ostrm, e_ostrm);
}

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_STRM_H

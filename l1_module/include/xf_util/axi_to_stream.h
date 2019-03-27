#ifndef XF_UTIL_AXI_TO_STRM_H
#define XF_UTIL_AXI_TO_STRM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"
#include <cstdio>// to be move to the common file of this library
// Forward decl

namespace xf {
namespace util {
namespace level1 {

// ---------------------- burst_read_axi_align_vec 1 ---------------------------------

template <int _WAxi, int _BurstLen, int size0>
void read_to_vec(
    ap_uint<_WAxi>* c0vec_ptr,
    const int nrow,
	const int scal_vec,
    hls::stream<ap_uint<_WAxi> >& c0vec_strm) {

  int nread = (nrow + scal_vec - 1) / scal_vec;

READ_TO_COL_VEC:
  for (int i = 0; i < nread; i += _BurstLen) {
#pragma HLS dataflow
    const int len = ((i + _BurstLen) > nread) ? (nread - i) : _BurstLen;
  READ_VEC0:
    for (int j = 0; j < len; ++j) {
#pragma HLS pipeline II = 1
      c0vec_strm.write(c0vec_ptr[i + j]);
    }
#ifndef __SYNTHESIS__
     printf("%d burst len %d\n", i / _BurstLen, len);
#endif
  }
}

template <int _WAxi, typename _TStrm>
void split_vec(
    hls::stream<ap_uint<_WAxi> >& c0vec_strm,
    const int nrow,
	const int scal_vec,
	const int size0,
    hls::stream<_TStrm >& c0_strm,
    hls::stream<bool>& e_strm) {

SPLIT_COL_VEC:
  for (int i = 0; i < nrow; i += scal_vec) {
#pragma HLS pipeline II = scal_vec
    ap_uint<_WAxi> c0vec = c0vec_strm.read();
    int n = (i + scal_vec) > nrow ? (nrow - i) : scal_vec;
    for (int j = 0; j < scal_vec; ++j) {
      ap_uint<8 * size0> c0 =
          c0vec.range(8 * size0 * (j + 1) - 1, 8 * size0 * j);
      if (j < n) {
        c0_strm.write((_TStrm)c0);
        e_strm.write(false);
      }
    }
  }
  e_strm.write(true);
}

/* @brief Loading data from AXI master to stream.
 *
 * This primitive assumes the width of AXI port is multiple of data width.
 * When data width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * Both AXI port and data are assumed to have width as multiple of 8-bit char.
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _WStrm width of the stream.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
 * @param offset offset from the beginning of the buffer, in number of char.
 */
template <int _WAxi, int _WStrm>
void burst_axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_WStrm> >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0);

/* @brief Loading data from AXI master to stream.
 *
 * This primitive assumes the width of AXI port is _SCAL_AXI times of alignment width.
 * When alignment width is less than AXI port width, the AXI port bandwidth
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
 * @tparam _WAlign data's alignment width in buffer.
 * @tparam _TStrm data's type.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
 * @param offset offset from the beginning of the buffer, in number of char.
 */
template <int _WAxi, int _BurstLen, int _SCAL_AXI,  typename _TStrm>
void burst_axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<_TStrm >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0){
	#pragma HLS dataflow
	  const int fifo_depth = _BurstLen * 2;
	  const int size0      = sizeof(_TStrm);
	  const int scal_vec    = _WAxi/(8*size0);

	  hls::stream<ap_uint<8 * size0 * scal_vec> > vec_strm;
	  ap_uint<8 * size0 * scal_vec>* 			 vec_ptr= rbuf + offset;
	#pragma HLS stream variable = c0vec_strm depth = fifo_depth

	  read_to_vec<_WAxi, _BurstLen, scal_vec>(
	      vec_ptr, num, scal_vec,
	      vec_strm);

	  split_vec<_WAxi, _TStrm>(
	      vec_strm, num, scal_vec, size0,
		  ostrm, e_ostrm);
}

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_STRM_H

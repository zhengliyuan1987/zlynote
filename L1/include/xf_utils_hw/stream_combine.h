#ifndef XF_UTILS_HW_STREAM_COMBINE_H
#define XF_UTILS_HW_STREAM_COMBINE_H

#include "xf_utils_hw/types.h"
#include "xf_utils_hw/enums.h"

/**
 * @file stream_combine.h
 * @brief combine multiple streams into a wider one to share end signal.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ======================================================

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief combine multiple streams into one, shift selected streams to LSB side.
 *
 * The first selected stream will be copied to LSB. The data within the stream
 * will not change the bit-order.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param alg algorithm selector for this function.
 */
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    lsb_side_t alg);

/**
 * @brief combine multiple streams into one, shift selected streams to MSB side.
 *
 * The first selected stream will be copied to MSB. The data within the stream
 * will not change the bit-order.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param alg algorithm selector for this function.
 */

template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    msb_side_t alg);

/**
 * @brief combine multiple streams into a wide one, align to LSB.
 *
 * The first selected stream will be copied to LSB. The data within the stream
 * will not change the bit-order.
 *
 * If ``_WOut > _WIn * _NStrm``, extra bits will be filled with zeros.
 *
 * @tparam _WIn input stream width.
 * @tparam _WOut output stream width, should be no less than _WIn * _NStrm.
 * @tparam _NStrm number of output stream.
 *
 * @param istrms input data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm output data stream.
 * @param e_ostrm end flag stream for the output.
 * @param alg algorithm selector for this function.
 */
template <int _WIn, int _WOut, int _NStrm>
void stream_combine(hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WOut> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    lsb_side_t alg); 

/**
 * @brief combine multiple streams into a wide one, align to MSB.
 *
 * The first selected stream will be copied to MSB. The data within the stream
 * will not change the bit-order.
 *
 * If ``_WOut > _WIn * _NStrm``, extra bits will be filled with zeros.
 *
 * @tparam _WIn input stream width.
 * @tparam _WOut output stream width, should be no less than _WIn * _NStrm.
 * @tparam _NStrm number of output stream.
 *
 * @param istrms input data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm output data stream.
 * @param e_ostrm end flag stream for the output.
 * @param alg algorithm selector for this function.
 */

template <int _WIn, int _WOut, int _NStrm>
void stream_combine(hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WOut> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    msb_side_t alg); 


} // utils_hw
} // common
} // xf

// Implementation ====================================================

namespace xf {
namespace common {
namespace utils_hw {

// comebin_left
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    lsb_side_t alg) {
  bool e = e_istrm.read();
  ap_uint<_NStrm> bb = select_cfg.read();
  bool b[_NStrm][_NStrm];
#pragma HLS array_partition variable = b complete dim = 1
  // b record output payload of colum
  ap_uint<_WIn * _NStrm> tmp_pld;
  ap_uint<_WIn> tmp[_NStrm][_NStrm];
#pragma HLS array_partition variable = tmp complete dim = 1

loop:
  while (!e) {
#pragma HLS pipeline II = 1
    for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
      tmp[0][i] = istrms[i].read();
      b[0][i] = bb[i];
    }
    for (int k = 0; k < _NStrm - 1; k++) {
#pragma HLS unroll
      int flag = 0;
      for (int j = 0; j < _NStrm; j++) {
#pragma HLS unroll
        if (b[k][j] == 1 && flag == 0) {
          // if b == 1,column stay the same
          b[k + 1][j] = b[k][j];
          tmp[k + 1][j] = tmp[k][j];
        }
        if ((b[k][j] == 0 && flag == 0) || (flag == 1)) {
          // if b == 0,columns all on the right side shift one step
          if (j < _NStrm - 1) {
            b[k + 1][j] = b[k][j + 1];
            tmp[k + 1][j] = tmp[k][j + 1];
          }
          // padding zero at the last column
          if (j == _NStrm - 1) {
            b[k + 1][_NStrm - 1] = 0;
            tmp[k + 1][_NStrm - 1] = 0;
          }
          flag = 1;
        }
      }
    }
    for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
      tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = tmp[_NStrm - 1][i];
    }
    {
      ostrm.write(tmp_pld);
      e_ostrm.write(0);
    }
    e = e_istrm.read();
  }
  e_ostrm.write(1);
}

// combine right
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    msb_side_t alg) {
  bool e = e_istrm.read();
  ap_uint<_NStrm> bb = select_cfg.read();
  bool b[_NStrm][_NStrm];
#pragma HLS array_partition variable = b complete dim = 1
  // b record output payload of colum
  ap_uint<_WIn * _NStrm> tmp_pld;
  ap_uint<_WIn> tmp[_NStrm][_NStrm];
#pragma HLS array_partition variable = tmp complete dim = 1

loop:
  while (!e) {
#pragma HLS pipeline II = 1
    for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
      tmp[0][i] = istrms[i].read();
      b[0][i] = bb[i];
    }
    for (int k = 0; k < _NStrm - 1; k++) {
#pragma HLS unroll
      int flag = 0;
      for (int j = 0; j < _NStrm; j++) {
#pragma HLS unroll
        if (b[k][j] == 1 && flag == 0) {
          // if b == 1,column stay the same
          b[k + 1][j] = b[k][j];
          tmp[k + 1][j] = tmp[k][j];
        }
        if ((b[k][j] == 0 && flag == 0) || (flag == 1)) {
          // if b == 0,columns all on the right side shift one step
          if (j < _NStrm - 1) {
            b[k + 1][j] = b[k][j + 1];
            tmp[k + 1][j] = tmp[k][j + 1];
          }
          // padding zero at the last column
          if (j == _NStrm - 1) {
            b[k + 1][_NStrm - 1] = 0;
            tmp[k + 1][_NStrm - 1] = 0;
          }
          flag = 1;
        }
      }
    }
    for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
      tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = tmp[_NStrm - 1][_NStrm - i - 1];
    }
    {
      ostrm.write(tmp_pld);
      e_ostrm.write(0);
    }
    e = e_istrm.read();
  }
  e_ostrm.write(1);
}

template <int _WIn, int _WOut, int _NStrm>
void stream_combine(hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,
                    hls::stream<ap_uint<_WOut> >& ostrm,
                    hls::stream<bool>& e_ostrm,
                    lsb_side_t alg)  {
/**
 * stream     
 *   A      0    4   8
 *   B      1    5   9
 *   C      2    6   a
 *   D      3    7   b
 *
 * output  3210 7654 ba98 
 * */

  const int max = _WIn*_NStrm > _WOut? _WIn*_NStrm : _WOut ;
  bool last= e_istrm.read();
  while(!last) {
   #pragma HLS pipeline II=1
   last=e_istrm.read();
   ap_uint<max> cmb=0;
   for(int i=0; i< _NStrm; ++i) {
    #pragma HLS unroll
    cmb.range( (i+1)*_WIn-1, i*_WIn) = istrms[i].read();
   }
   ap_uint<_WOut> o_cmb= cmb;
   ostrm.write(o_cmb);
   e_ostrm.write(false);

 }
  e_ostrm.write(true);
}


template <int _WIn, int _WOut, int _NStrm>
void stream_combine(hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,
                    hls::stream<ap_uint<_WOut> >& ostrm,
                    hls::stream<bool>& e_ostrm,
                    msb_side_t alg) {
/**
 * stream     
 *   A      0    4   8
 *   B      1    5   9
 *   C      2    6   a
 *   D      3    7   b
 *
 * output  0123 4567 89ab 
 * */


  const int max = _WIn*_NStrm > _WOut? _WIn*_NStrm : _WOut ;
  const int df  = _WOut - _WIn*_NStrm;
  const int rdf = df < 0?(-df) : df;
  const int w = _WIn*_NStrm ;
  bool last= e_istrm.read();
  while(!last) {
   #pragma HLS pipeline II=1
   last=e_istrm.read();
   ap_uint<max> cmb=0;
   for(int i=0,j=_NStrm-1; i< _NStrm; ++i,--j) {
    #pragma HLS unroll
    cmb.range( (j+1)*_WIn-1, j*_WIn) = istrms[i].read();
   }
   ap_uint<_WOut> o_cmb = (df>=0) ? (cmb << df) : (cmb >> rdf);
   ostrm.write(o_cmb);
   e_ostrm.write(false);
 }
  e_ostrm.write(true);
}

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_STREAM_COMBINE_H

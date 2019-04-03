#ifndef XF_UTIL_STREAM_COMBINE_H
#define XF_UTIL_STREAM_COMBINE_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl ======================================================

namespace xf {
namespace common {
namespace utils_hw {

/* @brief stream combine, shift selected stream to left.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param _op algorithm selector for this function.
 */
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_left_t _op);

/* @brief stream combine, shift selected stream to right.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param _op algorithm selector for this function.
 */

template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_right_t _op);

} // level1
} // util
} // xf

// Implementation ====================================================

namespace xf {
namespace common {
namespace utils_hw {

namespace details {
// comebin_left
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_left_t _op) {
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
    ap_uint<_NStrm> c = select_cfg.read();
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

                    combine_right_t _op) {
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
    ap_uint<_NStrm> c = select_cfg.read();
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

} // details
} // level1
} // util
} // xf

#endif // XF_UTIL_STREAM_DUP_H

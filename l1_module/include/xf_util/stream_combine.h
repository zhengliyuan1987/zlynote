#ifndef XF_UTIL_STREAM_COMBINE_H
#define XF_UTIL_STREAM_COMBINE_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl ======================================================

namespace xf {
namespace util {
namespace level1 {

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
                    hls::stream<bool> e_istrm,

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
                    hls::stream<bool> e_istrms[_NStrm],

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_right_t _op);


} // level1
} // util
} // xf

// Implementation ====================================================

namespace xf {
namespace util {
namespace level1 {

namespace details {
// TODO
} // details

// TODO

} // level1
} // util
} // xf

#endif // XF_UTIL_STREAM_DUP_H

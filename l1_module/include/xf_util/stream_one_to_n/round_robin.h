#ifndef XF_UTIL_STREAM_1N_RR_H
#define XF_UTIL_STREAM_1N_RR_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl ======================================================

namespace xf {
namespace util {
namespace level1 {

/* @brief stream distribute, in round-robin order from first output.
 *
 * The input stream is assumed to be conpact data to be splitted into
 * _WOutStrm wide data into output streams.
 *
 * @tparam _WInStrm input stream width.
 * @tparam _WOutStrm output stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param istrm input data stream.
 * @param e_istrm end flag stream for input data.
 * @param ostrms output data streams.
 * @param e_ostrms end flag streams, one for each output data stream.
 * @param _op algorithm selector.
 */
template <int _WInStrm, int _WOutStrm, int _NStrm>
void strm_one_to_n(hls::stream<ap_uint<_WInStrm> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<_WOutStrm> > ostrms[_NStrm],
                   hls::stream<bool> e_ostrms[_NStrm],
                   round_robin_t _op);

/* @brief stream distribute, in round-robin order from first output.
 *
 * @tparam _TIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param istrm input data stream.
 * @param e_istrm end flag stream for input data.
 * @param ostrms output data streams.
 * @param e_ostrms end flag streams, one for each output data stream.
 * @param _op algorithm selector.
 */
template <typename _TIn, int _NStrm>
void strm_one_to_n(hls::stream<_TIn>& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<_TIn> ostrms[_NStrm],
                   hls::stream<bool> e_ostrms[_NStrm],
                   round_robin_t _op);
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

template <int _WInStrm, int _WOutStrm, int _NStrm>
void strm_one_to_n(hls::stream<ap_uint<_WInStrm> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<_WOutStrm> > ostrms[_NStrm],
                   hls::stream<bool> e_ostrms[_NStrm],
                   load_balance_t _op) {
  // TODO
}

// -------------------------------------------------------------------

namespace details {
// TODO
} // details

template <typename _TIn, int _NStrm>
void strm_one_to_n(hls::stream<_TIn>& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<_TIn> ostrms[_NStrm],
                   hls::stream<bool> e_ostrms[_NStrm],
                   load_balance_t _op) {
  // TODO
}

} // level1
} // util
} // xf


#endif // XF_UTIL_STREAM_1N_RR_H

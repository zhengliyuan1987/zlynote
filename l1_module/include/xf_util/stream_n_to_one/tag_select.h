#ifndef XF_UTIL_STREAM_N1_TAG
#define XF_UTIL_STREAM_N1_TAG

#include "xf_util/types.h"
#include "xf_util/traits.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

// tag based collect, combine tag
template <int _WInStrm, int _WTagStrm>
void strm_n_to_one(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm + _WTagStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op);

// tag based collect, discard tag
template <typename _TIn, int _WTagStrm>
void strm_n_to_one(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op);

} // level1
} // utils
} // xf

// Implementation
// TODO

#endif // XF_UTIL_STREAM_N1_TAG

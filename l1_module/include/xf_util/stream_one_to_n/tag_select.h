#ifndef XF_UTIL_STREAM_1N_TAG_H
#define XF_UTIL_STREAM_1N_TAG_H

#include "xf_util/types.h"
#include "xf_util/enums.h"
#include "xf_util/traits.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

template <int _WInStrm, int _WTagStrm>
void strm_one_to_n(
    hls::stream<ap_uint<_WInStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    tag_select_t _flag);

template <int _TIn, int _WTagStrm>
void strm_one_to_n(
    hls::stream<_TIn>& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    tag_select_t _flag);

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_STREAM_1N_TAG_H

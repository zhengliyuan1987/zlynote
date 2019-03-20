#ifndef XF_UTIL_STRM_TO_AXI_H
#define XF_UTIL_STRM_TO_AXI_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

template <int _WAxi, int _WStrm>
void stream_to_axi(
    ap_uint<_WAxi>* wbuf,
    hls::stream<ap_uint<_WStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    const int num,
    const int offset = 0);

template <int _WAxi, int _WAlign, typename _TStrm>
void stream_to_axi(
    ap_uint<_WAxi>* wbuf,
    hls::stream<ap_uint<_TStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    const int num,
    const int offset = 0);

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_STRM_TO_AXI_H

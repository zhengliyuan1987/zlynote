#ifndef XF_UTIL_AXI_TO_STRM_H
#define XF_UTIL_AXI_TO_STRM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

template <int _WAxi, int _WStrm>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_WStrm> >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0);

template <int _WAxi, int _WAlign, typename _TStrm>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_TStrm> >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0);

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_STRM_H

#ifndef XF_UTIL_STREAM_N1_RR
#define XF_UTIL_STREAM_N1_RR

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

template <int _WInStrm, int _WOutStrm, int _NStrm>
void strm_n_to_one(hls::stream<ap_uint<_WInStrm> > istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<ap_uint<_WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _flag);

template <typename _TIn, int _NStrm>
void strm_n_to_one(hls::stream<_TIn> istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<_TIn>& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _flag);

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_STREAM_N1_RR

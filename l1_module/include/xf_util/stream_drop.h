#ifndef XF_UTIL_STRM_DROP_H
#define XF_UTIL_STRM_DROP_H

#ifndef XF_UTIL_STREAM_N1
#error "Please user upper level header stream_n_to_one.h"
#endif

// Forward decl

namespace xf {
namespace util {

template <typename _TIn, int _NStrm>
void strm_drop(hls::stream<_TIn> istrms[_NStrm],
               hls::stream<bool> e_istrms[_NStrm]);

template <typename _TIn>
void strm_drop(hls::stream<_TIn>& istrm,
               hls::stream<bool>& e_istrm);

} // util
} // xf

// TODO implementation
// ...

#endif // XF_UTIL_STRM_DROP_H

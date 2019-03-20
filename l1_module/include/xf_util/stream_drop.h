#ifndef XF_UTIL_STRM_DROP_H
#define XF_UTIL_STRM_DROP_H

#include "xf_util/types.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

/* @brief Drop multiple streams with end flag helpers.
 */
template <typename _TIn, int _NStrm>
void stream_drop(hls::stream<_TIn> istrms[_NStrm],
                 hls::stream<bool> e_istrms[_NStrm]);

/* @brief Drop one stream with end flag helper.
 */
template <typename _TIn>
void stream_drop(hls::stream<_TIn>& istrm,
                 hls::stream<bool>& e_istrm);

} // level1
} // util
} // xf

// TODO implementation
// ...

#endif // XF_UTIL_STRM_DROP_H

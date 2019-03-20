#ifndef XF_UTIL_STRM_DROP_H
#define XF_UTIL_STRM_DROP_H

// Forward decl

namespace xf {
namespace util {
namespace level1 {

/* @brief Synchronize streams for successor module.
 */
template <typename _TIn, int _NStrm>
void stream_sync(hls::stream<_TIn> istrms[_NStrm],
                 hls::stream<bool> e_istrms[_NStrm],
                 hls::stream<_TIn> ostrms[_NStrm],
                 hls::stream<bool> e_ostrm);

} // level1
} // util
} // xf

// TODO implementation
// ...

#endif // XF_UTIL_STRM_DROP_H

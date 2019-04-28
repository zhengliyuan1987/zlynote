#ifndef XF_UTILS_HW_STRM_DISCARD_H
#define XF_UTILS_HW_STRM_DISCARD_H

#include "xf_utils_hw/types.h"

// Forward decl

namespace xf {
namespace common {
namespace utils_hw {

/* @brief Discard multiple streams with end flag helper for each.
 */
template <typename _TIn, int _NStrm>
void stream_discard(hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool> e_istrms[_NStrm]);

/* @brief Discard multiple streams synchronized with one end flag
 */
template <typename _TIn, int _NStrm>
void stream_discard(hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool> e_istrm);

/* @brief Discard one stream with its end flag helper.
 */
template <typename _TIn>
void stream_discard(hls::stream<_TIn>& istrm,
                    hls::stream<bool>& e_istrm);

} // utils_hw
} // common
} // xf

// TODO implementation
// ...

#endif // XF_UTILS_HW_STRM_DISCARD_H

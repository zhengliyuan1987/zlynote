#ifndef XF_UTIL_STREAM_DUP_H
#define XF_UTIL_STREAM_DUP_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl ======================================================

namespace xf {
namespace common {
namespace utils_hw {

/* @brief stream duplication.
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
void stream_dup(hls::stream<_TIn>& istrm,
                hls::stream<bool>& e_istrm,
                hls::stream<_TIn> ostrms[_NStrm],
                hls::stream<bool> e_ostrms[_NStrm]);

} // utils_hw
} // common
} // xf

// Implementation ====================================================

namespace xf {
namespace common {
namespace utils_hw {

namespace details {
// TODO
} // details

template <typename _TIn, int _NStrm>
void stream_dup(hls::stream<_TIn>& istrm,
                hls::stream<bool>& e_istrm,
                hls::stream<_TIn> ostrms[_NStrm],
                hls::stream<bool> e_ostrms[_NStrm]) {
  // TODO
}

} // utils_hw
} // common
} // xf


#endif // XF_UTIL_STREAM_DUP_H

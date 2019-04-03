#ifndef XF_UTIL_AXI_TO_STRM_H
#define XF_UTIL_AXI_TO_STRM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl

namespace xf {
namespace common {
namespace utils_hw {

/* @brief Loading data from AXI master to stream.
 *
 * This primitive assumes the width of AXI port is multiple of data width.
 * When data width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * Both AXI port and data are assumed to have width as multiple of 8-bit char.
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _WStrm width of the stream.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
 * @param offset offset from the beginning of the buffer, in number of char.
 */
template <int _WAxi, int _WStrm>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_WStrm> >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0);

/* @brief Loading data from AXI master to stream.
 *
 * This primitive assumes the width of AXI port is multiple of alignment width.
 * When alignment width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * Both AXI port and alignment width are assumed to be multiple of 8.
 * The data width must be no greater than its alignment width.
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _WAlign data's alignment width in buffer.
 * @tparam _TStrm data's type.
 *
 * @param rbuf input AXI port.
 * @param ostrm output stream.
 * @param e_ostrm end flag for output stream.
 * @param num number of data to load from AXI port.
 * @param offset offset from the beginning of the buffer, in number of char.
 */
template <int _WAxi, int _WAlign, typename _TStrm>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_TStrm> >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0);

} // utils_hw
} // common
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_STRM_H

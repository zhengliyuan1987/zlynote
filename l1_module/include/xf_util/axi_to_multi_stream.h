#ifndef XF_UTIL_AXI_TO_MULTI_STRM_H
#define XF_UTIL_AXI_TO_MULTI_STRM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

/* @brief Loading multiple categories of data from one AXI master to streams.
 *
 * This primitive assumes the width of AXI port is multiple of alignment width.
 * When alignment width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * AXI port width and width of each type are assumed to be multiple of 8.
 * It is assumed that the data width in bits is ``8 * sizeof(T)``, and data
 * type can be casted from raw bits of matching width.
 *
 * This module assumes the data is tightly packed, so that the begining of
 * Type 2 data may be placed in one AXI port row with the end of Type 1 data.
 *
 * \rst
 * ::
 *
 *     AXI word [ elements of Type 1 ........................................ ]
 *     AXI word [ elements of Type 1 ..... end | begin elements of Type 2 ... ]
 *     AXI word [ elements of Type 2 ........................................ ]
 *
 * \endrst
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _WAlign data's alignment width in buffer.
 * @tparam _TStrm data's type.
 *
 * @param rbuf input AXI port.
 * @param ostrm1 output stream of type 1.
 * @param e_ostrm1 end flag for output stream of type 1.
 * @param ostrm2 output stream of type 2.
 * @param e_ostrm2 end flag for output stream of type 2.
 * @param ostrm3 output stream of type 3.
 * @param e_ostrm3 end flag for output stream of type 3.
 * @param num number of data to load from AXI port for each type.
 * @param offset offset for each type, in number of char.
 */
template <int _WAxi, int _BstLen, int _WStrm1, int _WStrm2, int _WStrm3>
void axi_to_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_WStrm1> >& ostrm1,
    hls::stream<bool>& e_ostrm1,
    hls::stream<ap_uint<_WStrm2> >& ostrm2,
    hls::stream<bool>& e_ostrm2,
    hls::stream<ap_uint<_WStrm3> >& ostrm3,
    hls::stream<bool>& e_ostrm3,
    const int num[3],
	const int len,
    const int offset[3]);

// TODO for 2 and for 4

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_MULTI_STRM_H

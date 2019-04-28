#ifndef XF_UTILS_HW_STREAM_N1_LB
#define XF_UTILS_HW_STREAM_N1_LB

/**
 * @file load_balance.h
 * @brief header for collect in round-roubin order.
 */

#include "xf_utils_hw/enums.h"
#include "xf_utils_hw/types.h"

// Forward decl

namespace xf {
namespace common {
namespace utils_hw {

template <int _WInStrm, int _WOutStrm, int _NStrm>
void stream_n_to_one(hls::stream<ap_uint<_WInStrm> > istrms[_NStrm],
                     hls::stream<bool> e_istrms[_NStrm],
                     hls::stream<ap_uint<_WOutStrm> >& ostrm,
                     hls::stream<bool>& e_ostrm,
                     load_balance_t _op);

template <typename _TIn, int _NStrm>
void stream_n_to_one(hls::stream<_TIn> istrms[_NStrm],
                     hls::stream<bool> e_istrms[_NStrm],
                     hls::stream<_TIn>& ostrm,
                     hls::stream<bool>& e_ostrm,
                     load_balance_t _op);

} // utils_hw
} // common
} // xf

// Implementation
// TODO

#endif // XF_UTILS_HW_STREAM_N1_LB

#ifndef XF_DATA_UTILS_STRM1N_LB_H
#define XF_DATA_UTILS_STRM1N_LB_H

#ifndef XF_DATA_UTILS_STRM1N_H
#error "Please user upper level header stream_one_to_n.h"
#endif

// Forward decl

namespace xf {
namespace data_utils {

template <int WInStrm, int WOutStrm, int NStrm>
void strm_one_to_n(hls::stream<ap_uint<WInStrm> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WOutStrm> > ostrms[NStrm],
                   hls::stream<bool> e_ostrms[NStrm],
                   load_balance_t _flag);

template <typename TIn, int NStrm>
void strm_one_to_n(hls::stream<TIn>& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<TIn> ostrms[NStrm],
                   hls::stream<bool> e_ostrms[NStrm],
                   load_balance_t _flag);

} // data_utils
} // xf

// TODO implementation
// ...

#endif // XF_DATA_UTILS_STRM1N_LB_H

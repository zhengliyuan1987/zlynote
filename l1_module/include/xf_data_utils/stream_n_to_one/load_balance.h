#ifndef XF_DATA_UTILS_STRMN1_LB
#define XF_DATA_UTILS_STRMN1_LB

#ifndef XF_DATA_UTILS_STRMN1
#error "Please user upper level header stream_n_to_one.h"
#endif

// Forward decl

namespace xf {
namespace data_utils {

template <int WInStrm, int WOutStrm, int NStrm>
void strm_n_to_one(hls::stream<ap_uint<WInStrm> > istrms[NStrm],
                   hls::stream<bool> e_istrms[NStrm],
                   hls::stream<ap_uint<WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm,
                   load_balance_t _flag);

template <typename TIn, int NStrm>
void strm_n_to_one(hls::stream<TIn> istrms[NStrm],
                   hls::stream<bool> e_istrms[NStrm],
                   hls::stream<TIn>& ostrm,
                   hls::stream<bool>& e_ostrm,
                   load_balance_t _flag);

} // data_utils
} // xf

// TODO implementation
// ...

#endif // XF_DATA_UTILS_STRMN1_LB

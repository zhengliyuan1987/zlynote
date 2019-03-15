#ifndef XF_DATA_UTILS_STRM1N_TAG_H
#define XF_DATA_UTILS_STRM1N_TAG_H

#ifndef XF_DATA_UTILS_STRM1N_H
#error "Please user upper level header stream_one_to_n.h"
#endif

// Forward decl

namespace xf {
namespace data_utils {

template <int WInStrm, int WTagStrm>
void strm_one_to_n(
    hls::stream<ap_uint<WInStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<WInStrm> > data_istrms[power_of_2<WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[power_of_2<WTagStrm>::value],
    tag_select_t _flag);

template <int TIn, int WTagStrm>
void strm_one_to_n(
    hls::stream<TIn>& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<TIn> data_istrms[power_of_2<WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[power_of_2<WTagStrm>::value],
    tag_select_t _flag);

} // data_utils
} // xf

// TODO implementation
// ...

#endif // XF_DATA_UTILS_STRM1N_TAG_H

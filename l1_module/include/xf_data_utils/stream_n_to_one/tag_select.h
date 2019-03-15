#ifndef XF_DATA_UTILS_STRMN1_TAG
#define XF_DATA_UTILS_STRMN1_TAG

#ifndef XF_DATA_UTILS_STRMN1
#error "Please user upper level header stream_n_to_one.h"
#endif

#include "xf_data_utils/utils.h"

// Forward decl

namespace xf {
namespace data_utils {

// tag based collect, combine tag
template <int WInStrm, int WTagStrm>
void strm_n_to_one(
    hls::stream<ap_uint<WInStrm> > data_istrms[power_of_2<WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[power_of_2<WTagStrm>::value],
    hls::stream<ap_uint<WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<WInStrm + WTagStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _flag);

// tag based collect, discard tag
template <typename TIn, int WTagStrm>
void strm_n_to_one(
    hls::stream<TIn> data_istrms[power_of_2<WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[power_of_2<WTagStrm>::value],
    hls::stream<ap_uint<WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _flag);

} // data_utils
} // xf

// TODO implementation
// ...

#endif // XF_DATA_UTILS_STRMN1_TAG

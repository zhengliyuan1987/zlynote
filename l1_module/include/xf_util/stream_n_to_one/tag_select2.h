#ifndef XF_UTIL_STREAM_N1_TAG
#define XF_UTIL_STREAM_N1_TAG

#include "xf_util/types.h"
#include "xf_util/traits.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

// tag based collect, combine tag
template <int _WInStrm, int _WTagStrm>
void strm_n_to_one(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm + _WTagStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op);

// tag based collect, discard tag
template <typename _TIn, int _WTagStrm>
void strm_n_to_one(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op);

} // level1
} // utils
} // xf

// Implementation
// TODO


namespace xf {
namespace util {
namespace level1 {
namespace details {

template <int _WInStrm, int _WTagStrm>
void strm_n_to_one_select(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm + _WTagStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm
    ) {


}

} // details
// tag based collect, combine tag
template <int _WInStrm, int _WTagStrm>
void strm_n_to_one(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm + _WTagStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op) {

 details::strm_n_to_one_select<_WInStrm, _WTagStrm>(
                                data_istrms,
                                e_data_istrms,
                                tag_istrm,
                                e_tag_istrm,
                                data_ostrm,
                                e_ostrm ) ;

}
//-------------------------------------------------------------------------------/
namespace details {

template <typename _TIn, int _WTagStrm>
void strm_n_to_one_select_type(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm) {
// TODO


}
} // details
// tag based collect, discard tag
template <typename _TIn, int _WTagStrm>
void strm_n_to_one(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool>& e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op) {

 details::strm_n_to_one_select_type<_TIn, _WTagStrm>(
                                data_istrms,
                                e_data_istrms,
                                tag_istrm,
                                e_tag_istrm,
                                data_ostrm,
                                e_ostrm ) ;

}

} // level1
} // utils
} // xf






#endif // XF_UTIL_STREAM_N1_TAG

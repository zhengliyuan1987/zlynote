#ifndef XF_DATA_UTILS_STRM_DROP_H
#define XF_DATA_UTILS_STRM_DROP_H

#ifndef XF_DATA_UTILS_STRMN1
#error "Please user upper level header stream_n_to_one.h"
#endif

// Forward decl

namespace xf {
namespace data_utils {

template <typename TIn, int NStrm>
void strm_drop(hls::stream<TIn> istrms[NStrm],
               hls::stream<bool> e_istrms[NStrm]);

template <typename TIn>
void strm_drop(hls::stream<TIn>& istrm,
               hls::stream<bool>& e_istrm);

} // data_utils
} // xf

// TODO implementation
// ...

#endif // XF_DATA_UTILS_STRM_DROP_H

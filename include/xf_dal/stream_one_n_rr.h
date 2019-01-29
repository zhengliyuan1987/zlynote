#ifndef _XF_DAL_STREAM_ONE_N_RR_H
#define _XF_DAL_STREAM_ONE_N_RR_H

namespace xf {
namespace dal {
struct round_robin_t {};
static const round_robin_t round_robin{};

template <int WInStrm, int WOutStrm, int NStrm>
void strm_one_to_n(hls::stream<ap_uint<WInStrm> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WOutStrm> > ostrms[NStrm],
                   hls::stream<bool> e_ostrms[NStrm],
                   round_robin_t _flag);

template <int WInStrm, int WOutStrm, int NStrm>
void strm_n_to_one(hls::stream<ap_uint<WInStrm> > istrms[NStrm],
                   hls::stream<bool> e_istrms[NStrm],
                   hls::stream<ap_uint<WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _flag);

} // dal
} // xf

#endif // _XF_DAL_STREAM_ONE_N_RR_H

#ifndef XF_UTIL_STREAM_COMBINE_H
#define XF_UTIL_STREAM_COMBINE_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl ======================================================

namespace xf {
namespace util {
namespace level1 {

/* @brief stream combine, shift selected stream to left.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrms end flag streams, one for each output data stream.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param _op algorithm selector for this function.
 */
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrms,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_left_t _op
                    ) {
		bool e;
		bool ecfg = 0;
		ap_uint<_NStrm> b;
		while (!(e = e_istrms.read())) {
			if(ecfg ==0){
				b = select_cfg.read();   //b record output payload of colums
				ecfg = 1;
			}	
			int out_N = 0;
			ap_uint<_WIn> tmp[_NStrm];
			ap_uint<_WIn * _NStrm> tmp_pld;
			for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
				if (b[i] == 1)
					out_N++;
				tmp[i] = istrms[i].read();
			}
		int count = 0;
			for (int i =0; i < _NStrm; i++){
#pragma HLS unroll
				if (b[i] == 1 && count < out_N){
					tmp_pld(_WIn * (count + 1) - 1, _WIn * count) = tmp[i];
					count++;	
				}
			}
			if(count < _NStrm)
			for(int i = count; i<_NStrm; i++){
				tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = (ap_uint<_WIn>)(0);
			}

		
			
			ostrm.write(tmp_pld);
			e_ostrm.write(0); 		
		}
		e_ostrm.write(1);
}

/* @brief stream combine, shift selected stream to right.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrms end flag streams, one for each output data stream.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param _op algorithm selector for this function.
 */
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrms,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_right_t _op
                    ) {
		bool e;
		bool ecfg = 0;
		ap_uint<_NStrm> b;
		while (!(e = e_istrms.read())) {
			if(ecfg == 0){
				b = select_cfg.read();   //b record output payload of colums
				ecfg = 1;
			}
			int out_N = 0;
			ap_uint<_WIn> tmp[_NStrm];
			ap_uint<_WIn * _NStrm> tmp_pld;
			for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
				if (b[i] == 1)
					out_N++;
				tmp[i] = istrms[i].read();
			}
		int count = 0;
			for (int i = 0; i < _NStrm; i++){
#pragma HLS unroll
				if (b[i] == 1 && count < out_N){
					tmp_pld(_WIn * (_NStrm - count) - 1, _WIn * (_NStrm - count - 1)) = tmp[i];
					count++;	
				}
			}
			if(count < _NStrm)
			for(int i = count; i<_NStrm; i++){
				tmp_pld(_WIn * (_NStrm - i) - 1, _WIn * (_NStrm - i - 1)) = (ap_uint<_WIn>)(0);
			}

		
			
			ostrm.write(tmp_pld);
			e_ostrm.write(0); 		
		}
		e_ostrm.write(1);
}
} // level1
} // util
} // xf

// Implementation ====================================================

namespace xf {
namespace util {
namespace level1 {

namespace details {
// TODO





}// details
// TODO
} // level1
} // util
} // xf

#endif // XF_UTIL_STREAM_DUP_H

#ifndef XF_UTILS_HW_STREAM_SHUFFLE_H
#define XF_UTILS_HW_STREAM_SHUFFLE_H

#include "xf_utils_hw/types.h"

/**
 * @file stream_shuffle.h
 * @brief Unidirectional cross-bar of streams.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ===============================================================

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Shuffle the contents from an array of streams to another.
 *
 * Suppose we have an array of 3 streams for R-G-B channels correspndingly,
 * and it is needed to shuffle B to Stream 0 and R to Stream 2.
 * This module can bridge this case with the configuration ``2, 1, 0``.
 * Here, ``2`` is the new index for data at old stream index ``0``,
 * and ``0`` is the new index for data at old stream index ``2``.
 *
 * The configuration is load once in one invocation, and reused until the end.
 * Totally ``_NStrm`` index integers will be read.
 *
 * @tparam _TIn input type.
 * @tparam _NStrm number of output stream.
 *
 * @param order_cfg the new order within the window, indexed from 0.
 * @param istrms input data streams.
 * @param e_istrm end flags for input.
 * @param ostrms output data streams.
 * @param e_ostrm end flag for output.
 */
template <typename _TIn, int _INStrm, int _ONstrm>
void stream_shuffle(hls::stream<ap_uint<4> > order_cfg[16],

                    hls::stream<_TIn> istrms[_INStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<_TIn> ostrms[_ONstrm],
                    hls::stream<bool>& e_ostrm); // TODO

} // utils_hw
} // common
} // xf

// Implementation =============================================================
namespace xf {
namespace common {
namespace utils_hw {

template <typename _TIn, int _INStrm, int _ONstrm>
void stream_shuffle(hls::stream<ap_uint<4> > order_cfg[16],

                    hls::stream<_TIn> istrms[_INStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<_TIn> ostrms[_ONstrm],
                    hls::stream<bool>& e_ostrm){

	bool e=e_istrm.read();
	ap_uint<4> route[_INStrm];
#pragma HLS ARRAY_PARTITION variable=route complete

	_TIn reg_i[_INStrm];
#pragma HLS ARRAY_PARTITION variable=reg_i complete
	_TIn reg_o[_INStrm];
#pragma HLS ARRAY_PARTITION variable=reg_o complete

	for(int i=0;i<_INStrm;i++){
#pragma HLS UNROLL
		route[i]=order_cfg[i].read();
	}

	while(!e){
#pragma HLS PIPELINE II=1

		for(int i=0;i<_INStrm;i++){
#pragma HLS UNROLL
			reg_i[i]=istrms[i].read();
		}

		for(int i=0;i<_INStrm;i++){
#pragma HLS UNROLL
			reg_o[route[i]]=reg_i[i];
		}

		for(int i=0;i<_ONstrm;i++){
#pragma HLS UNROLL
			ostrms[i].write(reg_o[i]);
		}

		e_ostrm.write(false);
		e=e_istrm.read();

	}
	e_ostrm.write(true);

}

} // utils_hw
} // common
} // xf


#endif // XF_UTILS_HW_STREAM_SHUFFLE_H

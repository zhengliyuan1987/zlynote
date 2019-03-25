#ifndef XF_UTIL_STREAM_DUP_H
#define XF_UTIL_STREAM_DUP_H

#include "xf_util/types.h"
//#include "xf_util/enums.h"

// Forward decl ======================================================

namespace xf {
namespace util {
namespace level1 {

/* @brief stream duplication.
 *
 * @tparam _TIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param istrm input data stream.
 * @param e_istrm end flag stream for input data.
 * @param ostrms output data streams.
 * @param e_ostrms end flag streams, one for each output data stream.
 * @param _op algorithm selector.
 */

template <typename _TIn, int _NStrm>
void stream_dup(hls::stream<_TIn>& istrm,
                hls::stream<bool>& e_istrm,
                hls::stream<_TIn> ostrms[_NStrm],
                hls::stream<bool> e_ostrms[_NStrm]) {
  // TODO
	bool e;
	while(!(e = e_istrm.read())) {
		_TIn tmp;
		tmp  = istrm.read();
		for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
		ostrms[i].write(tmp);
		e_ostrms[i].write(0);
		}	
	}	
	for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll	
		e_ostrms[i].write(1);
	}
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

 	
} // details
} // level1
} // util
} // xf


#endif // XF_UTIL_STREAM_DUP_H

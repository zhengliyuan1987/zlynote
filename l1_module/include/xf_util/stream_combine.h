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
 * @param e_istrm end flag for all input streams.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param _op algorithm selector for this function.
 */
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_left_t _op);


/* @brief stream combine, shift selected stream to right.
 *
 * @tparam _WIn input stream width.
 * @tparam _NStrm number of output stream.
 *
 * @param select_cfg one-hot encoded selection, LSB for istrms[0].
 * @param istrms output data streams.
 * @param e_istrm end flag for all input streams.
 * @param ostrm input data stream.
 * @param e_ostrm end flag stream for input data.
 * @param _op algorithm selector for this function.
 */

template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_right_t _op);

} // level1
} // util
} // xf

// Implementation ====================================================

namespace xf {
namespace util {
namespace level1 {

namespace details {
// comebin_left
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_left_t _op
                    ) {
		bool e = e_istrm.read();
		ap_uint<_NStrm> b[_NStrm+1];
		b[0] = select_cfg.read();   //b record output payload of colum
		ap_uint<_WIn * _NStrm> tmp_pld;
		ap_uint<_WIn> tmp[_NStrm + 1][_NStrm];
		int time = _NStrm;
		int count = 0;
		int delay = _NStrm;	
		while (!e) {
#pragma HLS pipeline II = 1
			e = e_istrm.read();
			for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
				tmp[0][i] = istrms[i].read();
			}
			
				
			for (int k = count; k >= 0; k--){
#pragma HLS unroll
				int flag = 0;
				for (int j = 0; j < _NStrm; j++){
#pragma HLS unroll
					if (b[k][j] == 1 && flag == 0){	
							//if b == 1,column stay the same
							b[k+1][j] = b[k][j];
							tmp[k+1][j] = tmp[k][j];
						
					}
					if (b[k][j] == 0 && flag == 0){	
							//if b == 0,columns all on the right side shift one step
						
							for (int s = j; s < _NStrm -1; s++){
#pragma HLS unroll	
								b[k+1][s] = b[k][s+1];
								tmp[k+1][s] = tmp[k][s+1];
							}
						
					
						//padding zero at the last column
						
						b[k+1][_NStrm - 1] = 0;
						tmp[k+1][_NStrm -1] = 0;
						
						flag = 1;
					}
				}
			}
			if(count < _NStrm){
				count++;
			}
			if(time > 0){
				time--;
			}
			for(int i = 0; i<_NStrm; i++){
#pragma HLS unroll
				tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = tmp[_NStrm][i];
			}
			if(time == 0)
			{
				ostrm.write(tmp_pld);
				e_ostrm.write(0);
			} 		
		}
// after input data is over, wait for _NStrm-1 period cycles to output 
	while (delay-1) {
#pragma HLS pipeline II = 1				
			 delay--;
			for (int k = count; k >= 0; k--){
#pragma HLS unroll
				int flag = 0;
				for (int j = 0; j < _NStrm; j++){
#pragma HLS unroll
					if (b[k][j] == 1 && flag == 0){	
							//if b == 1,column stay the same
							b[k+1][j] = b[k][j];
							tmp[k+1][j] = tmp[k][j];
						
					}
					if (b[k][j] == 0 && flag == 0){	
							//if b == 0,columns all on the right side shift one step
						
							for (int s = j; s < _NStrm -1; s++){
#pragma HLS unroll	
								b[k+1][s] = b[k][s+1];
								tmp[k+1][s] = tmp[k][s+1];
							}
						
					
						//padding zero at the last column
						
						b[k+1][_NStrm - 1] = 0;
						tmp[k+1][_NStrm -1] = 0;
						
						flag = 1;
					}
				}
			}
			if(count < _NStrm){
				count++;
			}
			if(time > 0){
				time--;
			}
			for(int i = 0; i<_NStrm; i++){
#pragma HLS unroll
				tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = tmp[_NStrm][i];
			}
			if(time == 0)
			{
				ostrm.write(tmp_pld);
				e_ostrm.write(0);
			} 		
		}
	e_ostrm.write(1);
}
//combine right
template <int _WIn, int _NStrm>
void stream_combine(hls::stream<ap_uint<_NStrm> >& select_cfg,

                    hls::stream<ap_uint<_WIn> > istrms[_NStrm],
                    hls::stream<bool>& e_istrm,

                    hls::stream<ap_uint<_WIn * _NStrm> >& ostrm,
                    hls::stream<bool>& e_ostrm,

                    combine_right_t _op
                    ) {
		bool e = e_istrm.read();
		ap_uint<_NStrm> b[_NStrm+1];
		b[0] = select_cfg.read();   //b record output payload of colum
		ap_uint<_WIn * _NStrm> tmp_pld;
		ap_uint<_WIn> tmp[_NStrm + 1][_NStrm];
		int time = _NStrm;
		int count = 0;
		int delay = _NStrm;	
		while (!e) {
#pragma HLS pipeline II = 1
			e = e_istrm.read();
								
			for (int i = 0; i < _NStrm; i++) {
#pragma HLS unroll
				tmp[0][i] = istrms[i].read();
			}
			
				
			for (int k = count; k >= 0; k--){
#pragma HLS unroll
				int flag = 0;
				for (int j = 0; j < _NStrm; j++){
#pragma HLS unroll
					if (b[k][j] == 1 && flag == 0){	
							//if b == 1,column stay the same
							b[k+1][j] = b[k][j];
							tmp[k+1][j] = tmp[k][j];
						
					}
					if (b[k][j] == 0 && flag == 0){	
							//if b == 0,columns all on the right side shift one step
						
							for (int s = j; s < _NStrm -1; s++){
#pragma HLS unroll	
								b[k+1][s] = b[k][s+1];
								tmp[k+1][s] = tmp[k][s+1];
							}
						
					
						//padding zero at the last column
						
						b[k+1][_NStrm - 1] = 0;
						tmp[k+1][_NStrm -1] = 0;
						
						flag = 1;
					}
				}
			}
			if(count < _NStrm){
				count++;
			}
			if(time > 0){
				time--;
			}
			for(int i = 0; i<_NStrm; i++){
#pragma HLS unroll
				tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = tmp[_NStrm][_NStrm - 1 - i];
			}
			if(time == 0)
			{
				ostrm.write(tmp_pld);
				e_ostrm.write(0);
			} 		
		}
// after input data is over, wait for _NStrm-1 period cycles to output 
	while (delay-1) {
#pragma HLS pipeline II = 1					
			 delay--;		
			for (int k = count; k >= 0; k--){
#pragma HLS unroll
				int flag = 0;
				for (int j = 0; j < _NStrm; j++){
#pragma HLS unroll
					if (b[k][j] == 1 && flag == 0){	
							//if b == 1,column stay the same
							b[k+1][j] = b[k][j];
							tmp[k+1][j] = tmp[k][j];
						
					}
					if (b[k][j] == 0 && flag == 0){	
							//if b == 0,columns all on the right side shift one step
						
							for (int s = j; s < _NStrm -1; s++){
#pragma HLS unroll	
								b[k+1][s] = b[k][s+1];
								tmp[k+1][s] = tmp[k][s+1];
							}
						
					
						//padding zero at the last column
						
						b[k+1][_NStrm - 1] = 0;
						tmp[k+1][_NStrm -1] = 0;
						
						flag = 1;
					}
				}
			}
			if(count < _NStrm){
				count++;
			}
			if(time > 0){
				time--;
			}
			for(int i = 0; i<_NStrm; i++){
#pragma HLS unroll
				tmp_pld(_WIn * (i + 1) - 1, _WIn * i) = tmp[_NStrm][_NStrm - 1 - i];
			}
			if(time == 0)
			{
				ostrm.write(tmp_pld);
				e_ostrm.write(0);
			} 		
		}
	e_ostrm.write(1);
}


}// details
// TODO
} // level1
} // util
} // xf

#endif // XF_UTIL_STREAM_DUP_H

#include "xf_utils_hw/stream_shuffle.h"
#include <iostream>

#define NUM_STRM 8

void top(
	hls::stream<int>& order_cfg,

	hls::stream<int> istrms[NUM_STRM],
	hls::stream<bool>& e_istrm,

	hls::stream<int> ostrms[NUM_STRM],
	hls::stream<bool>& e_ostrm
){
	xf::common::utils_hw::stream_shuffle<int,NUM_STRM>(
			order_cfg,
			istrms,
			e_istrm,
			ostrms,
			e_ostrm
	);
}

int main(){

	hls::stream<int> order_cfg;

	hls::stream<int> istrms[NUM_STRM];
	hls::stream<bool> e_istrm;

	hls::stream<int> ostrms[NUM_STRM];
	hls::stream<bool> e_ostrm;

	for(int i=0;i<NUM_STRM;i++){
		order_cfg.write(i);
	}

	for(int j=0;j<10;j++){
		for(int i=0;i<NUM_STRM;i++){
			istrms[i].write(i);
		}
		e_istrm.write(false);
	}
	e_istrm.write(true);

	top(
			order_cfg,
			istrms,
			e_istrm,
			ostrms,
			e_ostrm
	);

	e_ostrm.read();
	for(int i=0;i<NUM_STRM;i++){
		for(int j=0;j<10;j++){
			std::cout<<ostrms[i].read();
		}
		std::cout<<std::endl;
	}

	for(int j=0;j<10;j++){
		e_ostrm.read();
	}

	return 0;
}

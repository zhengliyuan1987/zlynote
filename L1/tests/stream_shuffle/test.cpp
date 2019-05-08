#include "xf_utils_hw/stream_shuffle.h"
#include <iostream>

#define NUM_INPUT  16
#define NUM_OUTPUT 8

void top(
	hls::stream<ap_uint<4> > order_cfg[16],

	hls::stream<int> istrms[NUM_INPUT],
	hls::stream<bool>& e_istrm,

	hls::stream<int> ostrms[NUM_OUTPUT],
	hls::stream<bool>& e_ostrm
){
	xf::common::utils_hw::stream_shuffle<int,NUM_INPUT,NUM_OUTPUT>(
			order_cfg,
			istrms,
			e_istrm,
			ostrms,
			e_ostrm
	);
}

int main(){

	hls::stream<ap_uint<4> > order_cfg[16];

	hls::stream<int> istrms[NUM_INPUT];
	hls::stream<bool> e_istrm;

	hls::stream<int> ostrms[NUM_INPUT];
	hls::stream<bool> e_ostrm;

	for(int i=0;i<NUM_INPUT;i++){
		order_cfg[i].write(i);
	}

	for(int j=0;j<10;j++){
		for(int i=0;i<NUM_INPUT;i++){
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
	for(int i=0;i<NUM_OUTPUT;i++){
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

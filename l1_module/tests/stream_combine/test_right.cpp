#include <vector>
#include <iostream>
#include <stdlib.h>

#include "xf_util/stream_combine.h"

#include "ap_int.h"
#include "hls_stream.h"
#define WIN 32
#define NSTRM 16 

extern "C" void dut(
    hls::stream<ap_uint<NSTRM> >& cfg,
    hls::stream<ap_uint<WIN> > in[NSTRM],
    hls::stream<bool>& ein,
    hls::stream<ap_uint<WIN * NSTRM> >& out,
    hls::stream<bool>& eout) {
  xf::util::level1::details::stream_combine(cfg, in, ein, out, eout, xf::util::combine_right_t());
}

#ifndef __SYNTHESIS__

// generate a random integer sequence between specified limits a and b (a<b);
uint rand_uint(uint a, uint b) {
	return rand() % (b - a +1) + a;
}

//generate test data
template <int _WIn, int _NStrm>
void generate_test_data( std::vector<ap_uint<_NStrm>> &testcfg, uint64_t len, std::vector<std::vector<ap_uint<_WIn>>> &testvector){
		ap_uint<_NStrm> cfg;
		for (int j = 0; j<_NStrm; j++){ 
			cfg[j] = (ap_uint<1>)(rand_uint(0,1));
			std::cout<<cfg[j];
		}
		std::cout<<std::endl;
		for (int i = 0; i<len; i++){
			std::vector<ap_uint<_WIn>> a;
			testcfg.push_back(cfg);
			if (i > 0){
				cfg = (ap_uint<_NStrm>)0;
			}
		for (int j = 0; j<_NStrm; j++){
			uint randnum = rand_uint(1,15);
			a.push_back((ap_uint<_WIn>)randnum);
std::cout<<randnum<<" ";
		}
		testvector.push_back(a);
std::cout<<std::endl;
		a.clear();
	}
	std::cout << " random test data generated! " << std::endl;
}

template <int _WIn, int _NStrm>
int test_function(int len){
	std::vector<ap_uint<_NStrm>> testcfg;
	std::vector<std::vector<ap_uint<_WIn>>> testvector;
	hls::stream<ap_uint<_NStrm>> incfg;
	hls::stream<ap_uint<_WIn>> in[_NStrm];
	hls::stream<ap_uint<_WIn * _NStrm>> out;
	hls::stream<bool> ein;
	hls::stream<bool> eout;
	
	//reference vector
	std::vector<ap_uint<_WIn * _NStrm>> refvec;
	//generate test data
	generate_test_data<_WIn, _NStrm>(testcfg, len, testvector);
	//prepare data to stream
	for (std::string::size_type i =0; i < len; i++){
		int count = 0;
		ap_uint<_WIn * _NStrm> tmp;
		incfg.write(testcfg[i]);
		for(int j = 0; j<_NStrm; j++){
			in[j].write(testvector[i][j]);
			if(testcfg[0][j] == 1){
				tmp((_NStrm - count) * _WIn - 1,(_NStrm - 1 - count) * _WIn) = testvector[i][j];
				count++;
			}
		}
		if(count < _NStrm)
		for(int j = count; j<_NStrm; j++){
			tmp((_NStrm - j) * _WIn - 1,(_NStrm - 1 - j) * _WIn) = (ap_uint<_WIn>)(0);
		}
		refvec.push_back(tmp);
		
			ein.write(0);
		
	}
	
		ein.write(1);
	
	//run hls::func
	dut(incfg, in, ein, out, eout);
	//compare hls::func and reference result
	int nerror = 0;
	//compare value
		for(std::string::size_type i = 0; i < len; i++){
			ap_uint<_WIn * _NStrm> out_res = out.read();
			for(int n = 0; n< _NStrm; n++){
				std::cout<<refvec[i].range(_WIn * (n + 1) - 1, _WIn * n)<<"   ";
			}
			for(int n = 0; n< _NStrm; n++){	
				std::cout<<out_res.range(_WIn * (n + 1) - 1, _WIn * n)<<"   ";
			}
			std::cout<<nerror<<std::endl;
			bool comp_res = (refvec[i] == out_res) ? 1 : 0;
			if(!comp_res){
				nerror++;
			}
		}
		//compare e flag
		for (std::string::size_type i =0; i < len; i++){
			bool estrm = eout.read();
			if(estrm){
				nerror++;
			}
		}
		bool estrm = eout.read();
		if(!estrm){
			nerror++;
		}
	return nerror;
}





int main(int argc, const char *argv[]) {
  int err = 0; // 0 for pass, 1 for error
  // TODO prepare cfg, in, ein; decl out, eout

err = test_function<32, 16>(32);

//  dut<32, 4>(cfg, in, ein, out, eout);
  // TODO check out, eout
if(err){
	std::cout<<"\nFAIL: nerror= "<<err<<" errors found.\n";
}
else{
	std::cout<<"\nPASS: no error found.\n";
}
  return err;

}

#endif

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "xf_util/stream_dup.h"

#include "ap_int.h"
#include "hls_stream.h"

typedef uint32_t TIN;
#define NSTRM 16

extern "C" void dut(
    hls::stream<TIN>& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<TIN> ostrms[NSTRM],
    hls::stream<bool> e_ostrms[NSTRM]) {
  xf::util::level1::details::stream_dup<TIN, NSTRM>(istrm, e_istrm, ostrms, e_ostrms);
}


#ifndef __SYNTHESIS__

// generate a random integer sequence between specified limits a and b (a<b);
uint rand_uint(uint a, uint b) {
	return rand() % (b - a +1) + a;
}

//generate test data
template <typename _TIn, int _NStrm>
void generate_test_data( uint64_t len, std::vector<_TIn> &testvector){
	for (int i = 0; i<len; i++){
		_TIn a;		
			uint randnum = rand_uint(1,15);
			testvector.push_back((_TIn)randnum);
std::cout<<randnum<<std::endl;
	}
	std::cout << " random test data generated! " << std::endl;
}

template <typename _TIn, int _NStrm>
int test_function(int len){
	std::vector<_TIn> testvector;
	hls::stream<_TIn> istrm;
	hls::stream<_TIn> ostrms[_NStrm];
	hls::stream<bool> e_istrm;
	hls::stream<bool> e_ostrms[_NStrm];
	
	//reference vector
	std::vector<_TIn> refvec;
	//generate test data
	generate_test_data<_TIn, _NStrm>(len, testvector);
	//prepare data to stream
	for (std::string::size_type i =0; i < len; i++){
		_TIn tmp = testvector[i];
		refvec.push_back(tmp);
		istrm.write(tmp);
		e_istrm.write(0);
	}
	e_istrm.write(1);
	//run hls::func
	dut(istrm, e_istrm, ostrms, e_ostrms);
	//compare hls::func and reference result
	int nerror = 0;
	//compare value
		for(std::string::size_type i = 0; i < len; i++){
			_TIn out_res[_NStrm];
				std::cout<<refvec[i]<<std::endl;
				for(int j = 0; j < _NStrm; j++){
					out_res[j] = ostrms[j].read();
					std::cout<<out_res[j]<<"   ";
					if(refvec[i]!=out_res[j]){
						nerror++;
					}							
				}
				std::cout<<nerror<<std::endl;
		}
		//compare e flag
		for (std::string::size_type i =0; i < len; i++){
			for(int j = 0; j < _NStrm; j++){
				bool estrm = e_ostrms[j].read();
				if(estrm){
					nerror++;
				}
			}		
		}
		for(int j = 0; j < _NStrm; j++){
			bool estrm = e_ostrms[j].read();
			if(!estrm){
				nerror++;
			}
		}	
	return nerror;
}





int main(int argc, const char *argv[]) {
  int err = 0; // 0 for pass, 1 for error
  // TODO prepare cfg, in, ein; decl out, eout

err = test_function<uint32_t, 16>(10);

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

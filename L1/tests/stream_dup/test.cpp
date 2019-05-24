#include <stdlib.h>
#include <iostream>
#include <vector>

#include "xf_utils_hw/stream_dup.h"

#include "ap_int.h"
#include "hls_stream.h"

typedef uint32_t TYPE;
#define NSTRM 16

extern "C" void dut0(hls::stream<TYPE>& istrm,
                    hls::stream<bool>& e_istrm,
                    hls::stream<TYPE> ostrms[NSTRM],
                    hls::stream<bool> e_ostrms[NSTRM]) {
  xf::common::utils_hw::stream_dup<TYPE, NSTRM>(
      istrm, e_istrm, ostrms, e_ostrms);
}

extern "C" void dut1(hls::stream<TYPE> istrm[8],
                    hls::stream<bool>& e_istrm,
                    hls::stream<TYPE> ostrms[8],
                    hls::stream<TYPE> dstrms[16][4],
                    hls::stream<bool>& e_ostrms) {
	const int choose[8]={0,1,2,3,-1,-1,-1,-1};
  xf::common::utils_hw::stream_dup<TYPE, 8,4,1>(
		  choose, istrm, e_istrm, ostrms, dstrms, e_ostrms);
}

#ifndef __SYNTHESIS__

int test_dut0(){
	int nerr = 0;
	int i;

	TYPE testdata[8][10];
	TYPE glddata[64][10];

	for(i=0;i<4;i++){
	  for(int j=0;j<10;j++){
		  testdata[i][j]=i*10+j;//rand()%1000;
		  for(int k=0;k<16;k++){
			  glddata[i*16+k][j]=testdata[i][j];
		  }
	  }
	}

	for(;i<8;i++){
	  for(int j=0;j<10;j++){
		  testdata[i][j]=i*10+j;//rand()%1000;
	  }
	}

	hls::stream<TYPE> istrm[8];
	hls::stream<bool> e_istrm[8];
	hls::stream<TYPE> ostrms[4][NSTRM];
	hls::stream<bool> e_ostrms[4][NSTRM];

	for(i=0;i<8;i++){
	  for(int j=0;j<10;j++){
		  istrm[i].write(testdata[i][j]);
		  e_istrm[i].write(0);
	  }
	  e_istrm[i].write(1);
	}

	for(i=0;i<4;i++){
	  dut0(istrm[i],e_istrm[i],ostrms[i],e_ostrms[i]);
	}

	for(i=0;i<4;i++){
	  for(int k=0;k<16;k++){
		  for(int j=0;j<10;j++){
			  e_ostrms[i][k].read();
			  if(glddata[i*16+k][j]!=ostrms[i][k].read()) nerr++;
		  }
		  e_ostrms[i][k].read();
	  }
	}

	for(;i<8;i++){
	  for(int j=0;j<10;j++){
		  e_istrm[i].read();
		  if(testdata[i][j]!=istrm[i].read()) nerr++;
	  }
	  e_istrm[i].read();
	}

	return nerr;
}

int test_dut1(){
	int nerr = 0;
	int i;

	TYPE testdata[8][10];
	TYPE glddata[64][10];

	for(i=0;i<4;i++){
	  for(int j=0;j<10;j++){
		  testdata[i][j]=i*10+j;//rand()%1000;
		  for(int k=0;k<16;k++){
			  glddata[i*16+k][j]=testdata[i][j];
		  }
	  }
	}

	for(;i<8;i++){
	  for(int j=0;j<10;j++){
		  testdata[i][j]=i*10+j;//rand()%1000;
	  }
	}

	hls::stream<TYPE> istrm[8];
	hls::stream<bool> e_istrm;
	hls::stream<TYPE> ostrms[8];
	hls::stream<TYPE> dstrms[16][4];
	hls::stream<bool> e_ostrms;

	std::cout<<"!!!"<<std::endl;

	for(int j=0;j<10;j++){
		for(i=0;i<8;i++){
		  istrm[i].write(testdata[i][j]);
		}
		e_istrm.write(0);
	}
	  e_istrm.write(1);
	std::cout<<"@@@"<<std::endl;
	dut1(istrm,e_istrm,ostrms,dstrms,e_ostrms);

	for(i=0;i<4;i++){
	  for(int k=0;k<16;k++){
		  for(int j=0;j<10;j++){
			  if(glddata[i*16+k][j]!=dstrms[k][i].read()) nerr++;
		  }
	  }
	}
	std::cout<<"###"<<std::endl;
	for(i=0;i<8;i++){
	  for(int j=0;j<10;j++){
		  e_istrm.read();
		  if(testdata[i][j]!=istrm[i].read()) nerr++;
	  }
	  e_istrm.read();
	}
	std::cout<<"$$$"<<std::endl;
	return nerr;
}


int main(int argc, const char* argv[]) {
	int nerr = 0;

	if(argv[1][0]=='0')
		nerr=nerr+test_dut0();
	else if(argv[1][0]=='1')
		nerr=nerr+test_dut1();

	// TODO check out, eout
	if (nerr) {
	std::cout << "\nFAIL: nerror= " << nerr << " errors found.\n";
	} else {
	std::cout << "\nPASS: no error found.\n";
	}
	return nerr;
}

#endif

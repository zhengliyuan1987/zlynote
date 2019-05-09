
#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_utils_hw/stream_split.h"

#define WOUT_STRM 16
#define NSTRM    64
#define NW     (NSTRM+1)
#define WIN_STRM  (WOUT_STRM*NW)
#define NS       1024*NW


void test_core_split_msb(hls::stream<ap_uint<WIN_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WOUT_STRM> > ostrms[NSTRM],
                   hls::stream<bool>& e_ostrm
                   ) {
  xf::common::utils_hw::stream_split<WIN_STRM, WOUT_STRM,NSTRM>(
                        istrm, e_istrm,
                        ostrms, e_ostrm,
                        xf::common::utils_hw::msb_side_t());
                        
}


int test_split_msb(){

   hls::stream<ap_uint<WIN_STRM> > data_istrm;
   hls::stream<bool> e_data_istrm;
   hls::stream<ap_uint<WOUT_STRM> > data_ostrms[NSTRM];
   hls::stream<bool> e_data_ostrm; 
  std::cout<<std::dec<< "WIN_STRM  = "<< WIN_STRM <<std::endl;
  std::cout<<std::dec<< "WOUT_STRM = "<< WOUT_STRM <<std::endl;
  std::cout<<std::dec<< "NSTRM     = "<< NSTRM <<std::endl;
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  int c=0;
  ap_uint<WIN_STRM> bd=0;
  ap_uint<WOUT_STRM> glds[NW][NS/NW]={0};
  for(int d=1; d<= NS; ++d)
  {
    int i = (d-1) % NW;
    ap_uint<WOUT_STRM> sd = d;
    glds[i][c] = d;
    bd.range((i+1)*WOUT_STRM-1, i*WOUT_STRM)= sd; 
    if (i==NW-1) {
      data_istrm.write(bd);
      bd = 0;
      ++c;
      e_data_istrm.write(false);
    }
  }

  e_data_istrm.write(true);

  test_core_split_msb( data_istrm, e_data_istrm,
                   data_ostrms, e_data_ostrm);
 int nerror = 0;
 bool last = e_data_ostrm.read();
 int p=0;

 while(!last) {
    last = e_data_ostrm.read();
    for ( int k=0, j=NW-1; k < NSTRM; ++k,--j) { 
      ap_uint<WOUT_STRM> sd = data_ostrms[k].read();
      if( sd != glds[j][p]) {
        nerror=1;
        std::cout<<"erro:"<< "  test data = "<< sd << "   "<< "gld data = "<< glds[k][p] << std::endl;
      }
   } // for
   p++;
 }
 if (p!=c)
   nerror=1; 
 if (nerror) {
        std::cout << "\nFAIL: " << nerror << "the order is wrong.\n";
 } 
 else {
        std::cout << "\nPASS: no error found.\n";
 }
 return nerror;

}

int main()
{
   return test_split_msb(); 
}


#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_utils_hw/stream_combine.h"

#define WIN_STRM  16 
#define WOUT_STRM 128
#define NS       1024*8
#define NSTRM    4


void test_core_comb_lsb(hls::stream<ap_uint<WIN_STRM> > istrms[NSTRM],
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WOUT_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm
                   ) {
  xf::common::utils_hw::stream_combine<WIN_STRM, WOUT_STRM,NSTRM>(
                        istrms, e_istrm,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::lsb_side_t());
                        
}


int test_comb_lsb(){

   hls::stream<ap_uint<WIN_STRM> > data_istrms[NSTRM];
   hls::stream<bool> e_data_istrm;
   hls::stream<ap_uint<WOUT_STRM> > data_ostrm;
   hls::stream<bool> e_data_ostrm;
   
  std::cout<<std::dec<< "WIN_STRM  = "<< WIN_STRM <<std::endl;
  std::cout<<std::dec<< "WOUT_STRM = "<< WOUT_STRM <<std::endl;
  std::cout<<std::dec<< "NSTRM     = "<< NSTRM <<std::endl;
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  for(int d=1; d<= NS; ++d)
  { 
    ap_uint<WIN_STRM> id =(d-1) % NSTRM; 
    data_istrms[id].write(d);
    if(id==0)
       e_data_istrm.write(false);
#if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
    std::cout<< "id = "<< id << "  "<< "d="<<d << std::endl;
#endif
  }

  e_data_istrm.write(true);

  test_core_comb_lsb( data_istrms, e_data_istrm,
                   data_ostrm, e_data_ostrm);
 int nerror = 0;
 bool last= e_data_ostrm.read();
 ap_uint<WIN_STRM> gld=0;
 while(!last) {
    last= e_data_ostrm.read();
    ap_uint<WOUT_STRM> buff = data_ostrm.read();
    for ( int k=0; k < NSTRM; ++k) { 
      ap_uint<WIN_STRM> d = buff.range( (k+1)*WIN_STRM-1, k*WIN_STRM);
      gld = gld + 1;
      if( d != gld) {
        nerror=1;
        std::cout<<"erro:"<< "  test data = "<< d << "   "<< "gld data = "<< gld  << std::endl;
      }
   }
 }
  
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
   return test_comb_lsb(); 
}



#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_utils_hw/stream_one_to_n/round_robin.h"
#include "xf_utils_hw/stream_n_to_one/round_robin.h"

#define WIN_STRM  512 
#define WOUT_STRM 64
#define NS       1024*8
#define NSTRM    8


void test_core(hls::stream<ap_uint<WIN_STRM> >& data_istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WIN_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
#pragma dataflow

     hls::stream<ap_uint<WOUT_STRM> > data_inner_strms[NSTRM];
#pragma HLS stream variable = data_inner_strms depth = 32              
//#pragma HLS array_partition variable = data_inner_strms dim = 1
     hls::stream<bool> e_data_inner_strms[NSTRM];
#pragma HLS stream variable = e_data_inner_strms depth = 32              
//#pragma HLS array_partition variable = e_data_inner_strms dim = 1

  xf::common::utils_hw::stream_one_to_n<WIN_STRM, WOUT_STRM,NSTRM>(
                         data_istrm,  e_istrm,
                         data_inner_strms, e_data_inner_strms,
                         xf::common::utils_hw::round_robin_t());
                        
  xf::common::utils_hw::stream_n_to_one<WOUT_STRM, WIN_STRM,NSTRM>(
                        data_inner_strms, e_data_inner_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::round_robin_t());
}


int test() {

   hls::stream<ap_uint<WIN_STRM> > data_istrm;
   hls::stream<bool> e_istrm;
   hls::stream<ap_uint<WIN_STRM> > ostrm;
   hls::stream<bool> e_ostrm;
   
   int tempa= WIN_STRM*NS/WOUT_STRM;
   int tempb= tempa*WOUT_STRM;
   int comp_count= tempb/WIN_STRM;// comp_count<=NS, last data may not equal NS-1 

  std::cout<<std::dec<< "WIN_STRM  = "<< WIN_STRM <<std::endl;
  std::cout<<std::dec<< "WOUT_STRM = "<< WOUT_STRM <<std::endl;
  std::cout<<std::dec<< "NSTRM     = "<< NSTRM <<std::endl;
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  for(int d=0; d< NS; ++d)  { 
    ap_uint<WIN_STRM> id =d % NSTRM; 
    data_istrm.write(d);
    e_istrm.write(false);
#if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
    std::cout<< "id = "<< id << "  "<< "d="<<d << std::endl;
#endif
  } 
  e_istrm.write(true);

  test_core( data_istrm, e_istrm,
              ostrm, e_ostrm);

  int nerror=0;
  int count=0;
  bool first=true;
  ap_uint<WIN_STRM> last_data; 
  
  while(!e_ostrm.read())  {
    ap_uint<WIN_STRM> d = ostrm.read(); 
    if ( first ) {
     first=false;
    }
    else {
      if (count <= comp_count && d != last_data+1) {
        nerror=1;
        std::cout<< "erro: last_data="<< last_data <<", "<< "current data="<<d <<std::endl;
       } 
     }
     last_data=d;
     count++;
  }
  if (nerror) {
     std::cout << "\nFAIL: " << nerror << "the order is wrong.\n";
  } 
  else {
       std::cout << "\nPASS: no error found.\n";
  }
  return nerror;
}

int main() {
 return test();
}


#include <vector>   // std::vector
#include <iostream>
#include "xf_util/stream_one_to_n/tag_select.h"

#define WIN_STRM  8 
#define WTAG_STRM 3
#define NS (WIN_STRM) * (WIN_STRM)-5
#define NSTRM (1 << WTAG_STRM )

void test_core_1_n_select(hls::stream<ap_uint<WIN_STRM> >& data_istrm,
                   hls::stream<bool>& e_data_istrm,
                   hls::stream<ap_uint<WTAG_STRM> >& tag_istrm,
                   hls::stream<bool>& e_tag_istrm,
                   hls::stream<ap_uint<WIN_STRM> > data_ostrms[NSTRM],
                   hls::stream<bool> e_data_ostrms[NSTRM]) {
  
  xf::common::utils_hw::strm_one_to_n<WIN_STRM, WTAG_STRM>(
                         data_istrm, 
                         e_data_istrm,
                         tag_istrm,   
                         e_tag_istrm,
                         data_ostrms, 
                         e_data_ostrms,
                         xf::common::utils_hw::tag_select_t());
}


int test_1_n_select() {

   hls::stream<ap_uint<WIN_STRM> > data_istrm;
   hls::stream<bool> e_data_istrm;
   hls::stream<ap_uint<WTAG_STRM> > tag_istrm;
   hls::stream<bool> e_tag_istrm;
   hls::stream<ap_uint<WIN_STRM> > data_ostrms[NSTRM];
   hls::stream<bool> e_data_ostrms[NSTRM];

   std::cout<< "NS    = "<< NS <<std::endl;
   std::cout<< "NSTRM = "<< NSTRM <<std::endl;
   std::cout<< "pow   = "<< PowerOf2<WTAG_STRM>::value <<std::endl;
   for(int j=0; j< NS; ++j) {
      ap_uint<WTAG_STRM> id =j % NSTRM; 
      data_istrm.write(j);
      e_data_istrm.write(false);
      tag_istrm.write(id);
      e_tag_istrm.write(false);
     #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
      std::cout<< "id = "<< id << "  "<< "j="<<j<< std::endl;
     #endif
   }
   e_data_istrm.write(true);
   e_tag_istrm.write(true);
   test_core_1_n_select( data_istrm,e_data_istrm, 
                   tag_istrm, e_tag_istrm,
                   data_ostrms, e_data_ostrms);
  int nerror=0;
  unsigned int count = 0;
  for(int id=0;id< NSTRM; ++id) { 
    unsigned int ls = 0;
    while( false==e_data_ostrms[id].read()) {
       ap_uint<WIN_STRM> data = data_ostrms[id].read();
       if( data % NSTRM != id) {
         nerror=1;
         std::cout<<" erro :  tag="<< id <<" "<<"data= "<< data <<std::endl;
       }
       else {
         #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
         std::cout<<"tag="<< id  <<" "<<"data= "<< data <<std::endl;
         #endif
       }
       ls++;
    }
    count += ls;
    std::cout<< "the length of stream_"<< id <<": " << ls << std::endl;
  } 
  if(count != NS) {
     nerror=1;
  }
  if (nerror)  {
       std::cout << "\nFAIL: " << nerror << "the order is wrong.\n";
  } else  {
       std::cout << "\nPASS: no error found.\n";
  }
  return nerror;
}

int main()
{
  return test_1_n_select();

}


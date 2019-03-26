
#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_util/stream_n_to_one/tag_select.h"

#define WIN_STRM 8 
#define WTAG_STRM 3
#define NS       128
#define NSTRM   (1<<(WTAG_STRM))


void test_core_n_1(hls::stream<ap_uint<WIN_STRM> > data_istrms[NSTRM],
                   hls::stream<bool> e_data_istrms[NSTRM],
                   hls::stream<ap_uint<WTAG_STRM> >& tag_istrm,
                   hls::stream<bool>& e_tag_istrm,
                   hls::stream<ap_uint<WIN_STRM> >& data_ostrm,
                   hls::stream<bool>& e_data_ostrm)
{


 xf::util::level1::strm_n_to_one<WIN_STRM, WTAG_STRM>(
                   data_istrms, e_data_istrms, 
                   tag_istrm,   e_tag_istrm,
                   data_ostrm,  e_data_ostrm,
                   xf::util::tag_select_t());

}


int test_n_1(){

  hls::stream<ap_uint<WIN_STRM> > data_istrms[NSTRM];
  hls::stream<bool>  e_data_istrms[NSTRM];
  hls::stream<ap_uint<WTAG_STRM> > tag_istrm;
  hls::stream<bool>  e_tag_istrm;
  hls::stream<ap_uint<WIN_STRM > > data_ostrm;
  hls::stream<bool>  e_data_ostrm;

 for(int j=0; j< NS; ++j) {
    int id =j % NSTRM; 
    data_istrms[id].write(j);
    e_data_istrms[id].write(false);
    tag_istrm.write(id);
    e_tag_istrm.write(false);
  #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
   std::cout<<"id="<< id <<" "<<"data= "<< j <<std::endl;
  #endif
 }
 
 for(int i=0; i< NSTRM; ++i)
    e_data_istrms[i].write(true);

 e_tag_istrm.write(true);

 test_core_n_1( data_istrms, e_data_istrms,
                   tag_istrm, e_tag_istrm,
                   data_ostrm, e_data_ostrm);
  int nerror=0;
  unsigned int ls=0;
   while(!e_data_ostrm.read())  {
      ap_uint<WIN_STRM > data = data_ostrm.read();
      if(  data != ls ) {
         nerror=1;
         std::cout<<" erro :  tag="<< ls %NSTRM <<" "<<"data= "<< data <<std::endl;
       }
       else {
         #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
          std::cout<<"tag="<< ls % NSTRM <<" "<<"data= "<< data <<std::endl;
         #endif
       }
      ls++;
   } // while
  std::cout<< "the length of the collected stream: "<< ls << std::endl;
  if(ls != NS) 
    nerror=1; 

    if (nerror) {
        std::cout << "\nFAIL: " << nerror << "the order is wrong.\n";
    } else {
        std::cout << "\nPASS: no error found.\n";
    }
    return nerror;
}

int main()
{
  return test_n_1();
}

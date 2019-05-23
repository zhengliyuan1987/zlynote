

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "code.h"

// double input as output 
ap_uint<W_STRM> compute(int d) {
   ap_uint<W_STRM> nd = d;
   nd = nd <<1;
   return nd;
};

int test() {

  hls::stream<ap_uint<W_STRM> > istrm;
  hls::stream<bool> e_istrm;
  hls::stream<ap_uint<W_TAG> > tg_strm;
  hls::stream<bool> e_tg_strm;
  hls::stream<ap_uint<W_STRM> > ostrm;
  hls::stream<bool> e_ostrm;
  

  std::cout<<std::dec<< "W_STRM  = "<< W_STRM <<std::endl;
  std::cout<<std::dec<< "W_TAG   = "<< W_TAG <<std::endl;
  std::cout<<std::dec<< "NTAG    = "<< NTAG <<std::endl;
  std::cout<<std::dec<< "NS      = "<< NS <<std::endl;
  for(int d=0; d< NS; ++d)  { 
    istrm.write(d);
    e_istrm.write(false);
    ap_uint<W_TAG> tg = NTAG - d % NTAG ;
    tg_strm.write(d);
    e_tg_strm.write(false);
  } 
  e_istrm.write(true);
  e_tg_strm.write(true);

    hls::stream<ap_uint<W_TAG> > du_tag_strms[2];
#pragma HLS stream variable = du_tag_strms depth = 8              
     hls::stream<bool> e_du_tag_strms[2];
#pragma HLS stream variable = e_du_tag_strms depth = 8              

 xf::common::utils_hw::stream_dup<ap_uint<W_TAG>, 2>(
                     tg_strm, e_tg_strm,
                     du_tag_strms, e_du_tag_strms);

 

  test_core( istrm, e_istrm,
             du_tag_strms, e_du_tag_strms,
//             tg_strm,e_tg_strm,
             ostrm, e_ostrm);

  int nerror=0;
  int count=0;
  bool first=true;
  
  while(!e_ostrm.read()) {
    ap_uint<W_STRM> d = ostrm.read(); 
      ap_uint<W_STRM> gld = count; //compute(count);
      if (count <= NS && d != gld) {
        nerror=1;
        std::cout<< "erro: "<<"c="<<count<<", gld="<< gld <<", "<< " data="<<d <<std::endl;
       } 
     count++;
  } // while
  std::cout << "\n total read: " << count <<std::endl;
  if( count != NS) {
    nerror=1;
    std::cout << "\n error:  total read = " << count << ", NS = " << NS <<std::endl;
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

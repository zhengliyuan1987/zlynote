

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "code.hpp"


int test() {

  hls::stream<ap_uint<W_STRM> > istrm;
  hls::stream<bool> e_istrm;
  hls::stream<ap_uint<W_TAG> > tg_strm;
  hls::stream<bool> e_tg_strm;
  hls::stream<ap_uint<W_TAG> > du_tag_strms[2];
  hls::stream<bool> e_du_tag_strms[2];
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

 //duplicate, one  for one to n, another for n to one.
  xf::common::utils_hw::stream_dup<ap_uint<W_TAG>, 2>(
                     tg_strm, e_tg_strm,
                     du_tag_strms, e_du_tag_strms);
 // process
  test_core( istrm, e_istrm,
             du_tag_strms, e_du_tag_strms,
             ostrm, e_ostrm);

  // fetch back and check
  int nerror=0;
  int count=0;
  while(!e_ostrm.read()) {
    ap_uint<W_STRM> d = ostrm.read(); 
      ap_uint<W_STRM> gld = count+1; 
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

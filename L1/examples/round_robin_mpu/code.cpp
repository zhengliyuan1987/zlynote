

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "code.h"


ap_uint<WOUT_STRM> compute(int d) {
   ap_uint<WOUT_STRM> nd = d;
   nd = nd <<1;
   return nd;
};

int test() {

   hls::stream<ap_uint<WIN_STRM> > data_istrm;
   hls::stream<bool> e_istrm;
   hls::stream<ap_uint<WIN_STRM> > ostrm;
   hls::stream<bool> e_ostrm;
   
   int tempa= WIN_STRM*NS/WOUT_STRM;
   int tempb= tempa*WOUT_STRM;
   int comp_count= tempb/WIN_STRM; 

  std::cout<<std::dec<< "WIN_STRM  = "<< WIN_STRM <<std::endl;
  std::cout<<std::dec<< "WOUT_STRM = "<< WOUT_STRM <<std::endl;
  std::cout<<std::dec<< "NSTRM     = "<< NSTRM <<std::endl;
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  for(int d=0; d< NS; ++d)  { 
    data_istrm.write(d);
    e_istrm.write(false);
  } 
  e_istrm.write(true);

  round_robin_mpu( data_istrm, e_istrm,
              ostrm, e_ostrm);

  int nerror=0;
  int count=0;
  bool first=true;
  
  while(!e_ostrm.read())  {
    ap_uint<WIN_STRM> d = ostrm.read(); 
      ap_uint<WOUT_STRM> gld = compute(count);
      if (count <= comp_count && d != gld) {
        nerror=1;
        std::cout<< "erro: "<<"c="<<count<<", gld="<< gld <<", "<< "current data="<<d <<std::endl;
       } 
     count++;
  }
  std::cout << "\n read: " << count <<std::endl;
  if( count != comp_count) {
    nerror=1;
    std::cout << "\n error  read: " << count <<std::endl;
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

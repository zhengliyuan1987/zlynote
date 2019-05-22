

#include <vector>
#include <iostream>
#include <stdlib.h>
#include "code.h"

/*
//#include "xf_utils_hw/stream_one_to_n/round_robin.h"
//#include "xf_utils_hw/stream_n_to_one/round_robin.h"
//#include "xf_utils_hw/stream_one_to_n.h"
//#include "xf_utils_hw/stream_n_to_one.h"

#define W_STRM  512 
#define W_PU 64
#define NS       (1024*8)
#define NPU    8
*/



int test() {

   hls::stream<ap_uint<W_STRM> > data_istrm;
   hls::stream<bool> e_istrm;
   hls::stream<ap_uint<W_STRM> > ostrm;
   hls::stream<bool> e_ostrm;
   
   int tempa= W_STRM*NS/W_PU;
   int tempb= tempa*W_PU;
   int comp_count= tempb/W_STRM;// comp_count<=NS, last data may not equal NS-1 

  std::cout<<std::dec<< "W_STRM  = "<< W_STRM <<std::endl;
  std::cout<<std::dec<< "W_PU = "<< W_PU <<std::endl;
  std::cout<<std::dec<< "NPU     = "<< NPU <<std::endl;
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  ap_uint<W_STRM> gld=0; 
  for(int i=0; i< NS; ++i)  { 

    ap_uint<W_PRC> p = i;
    ap_uint<W_DSC> d = i%10;
    ap_uint<W_PU> data;
    
    data.range(W_PRC-1,0)= p ;
    data.range(W_DSC+W_PRC-1,W_PRC)= d;
    data_istrm.write(data);
    e_istrm.write(false);
    
    ap_uint<W_PU> nd = update_data(data) ;
    gld += mult(nd);
  } 
  e_istrm.write(true);

  test_core( data_istrm, e_istrm,
              ostrm, e_ostrm);

  int nerror=0;
  int count=0;
  bool first=true;
  ap_uint<W_STRM> total=0; 
  
  while(!e_ostrm.read())  {
    ap_uint<W_STRM> d = ostrm.read(); 
    total += mult(d); 
    count++;
  }
  if( total != gld){
    nerror=1;
    std::cout << "\n error: total" << total << "   gld="<<gld <<std::endl;
  }
   
  std::cout << "\n read: " << count <<std::endl;
  if( count != NS){
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

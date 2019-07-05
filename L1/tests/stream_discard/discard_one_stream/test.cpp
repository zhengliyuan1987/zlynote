
#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_utils_hw/stream_discard.hpp"

#define NS       1024*8


void test_core_discard_stream(hls::stream<float>& istrm,
                   hls::stream<bool>& e_istrm ) {
 
  xf::common::utils_hw::stream_discard <float> (
                        istrm, e_istrm);
                        
}


int test_discard_stream(){

  hls::stream<float> data_istrm;
  hls::stream<bool> e_data_istrm;
   
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  for(int d=1; d<= NS; ++d)
  { 
    data_istrm.write(d+0.5);
    e_data_istrm.write(false);
  }

  e_data_istrm.write(true);

  test_core_discard_stream( data_istrm, e_data_istrm);
  std::cout << "\nPASS: no error found.\n";
  return 0;
}

int main()
{
   return test_discard_stream(); 
}


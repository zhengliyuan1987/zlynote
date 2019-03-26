
#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_util/stream_n_to_one/round_robin.h"

#define WIN_STRM  16 
#define WOUT_STRM 256
#define NS       1024*8
#define NSTRM    16


void test_core_n_1(hls::stream<float > istrms[NSTRM],
                   hls::stream<bool> e_istrms[NSTRM],
                   hls::stream<float >& ostrm,
                   hls::stream<bool>& e_ostrm
                   ) {
  xf::util::level1::strm_n_to_one<float,NSTRM>(
                        istrms, e_istrms,
                        ostrm, e_ostrm,
                        xf::util::round_robin_t());
                        
}


int test_n_1(){

   hls::stream<float > data_istrms[NSTRM];
   hls::stream<bool> e_data_istrms[NSTRM];
   hls::stream<float> data_ostrm;
   hls::stream<bool> e_data_ostrm;
   

  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  std::cout<<std::dec<< "input NS = "<< NS <<" float data "<<std::endl;
  for(int d=0; d< NS; ++d)
  { 
    int id =d % NSTRM; 
    float data = d*d+0.5;
    data_istrms[id].write(data);
    e_data_istrms[id].write(false);
#if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
    std::cout<< "id = "<< id << "  "<< "d="<<d << std::endl;
#endif
  }

 for(int i=0; i<NSTRM; ++i)
   e_data_istrms[i].write(true);

  test_core_n_1( data_istrms, e_data_istrms,
                   data_ostrm, e_data_ostrm);
 int nerror = 0;
 int n      = 0;
 float gld  = 0;
 bool last= e_data_ostrm.read();
 while(!last) {
   // get num_out*WOUT_STRM bits from stream 
    last= e_data_ostrm.read();
    float data = data_ostrm.read();
    gld = n * n + 0.5;
    ++n;
    if ( data != gld) {
      nerror=1;
      std::cout<< "data = "<< data << std::endl;
      std::cout<< "gld  = "<< gld << std::endl;
     }
 } // while(!last)
  std::cout<<std::dec<< "output n = "<< n <<" float data"<<std::endl;
  nerror = (n!=(NS))? 1 : nerror;
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
   return test_n_1(); 
}


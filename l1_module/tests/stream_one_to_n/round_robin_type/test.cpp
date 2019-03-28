
#include <vector>
#include <iostream>
#include <stdlib.h>


#include "xf_util/stream_one_to_n/round_robin.h"

#define WIN_STRM  512 
#define WOUT_STRM 16
#define NS       1024*8
#define NSTRM    8


void test_core_1_n(hls::stream<float>& data_istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<float> data_ostrms[NSTRM],
                   hls::stream<bool> e_data_ostrms[NSTRM])
{
  xf::util::level1::strm_one_to_n<float,NSTRM>(
                         data_istrm,  e_istrm,
                         data_ostrms, e_data_ostrms,
                         xf::util::round_robin_t());
                        
}


int test_1_n(){

   hls::stream<float> data_istrm;
   hls::stream<bool> e_istrm;
   hls::stream<float> data_ostrms[NSTRM];
   hls::stream<bool> e_data_ostrms[NSTRM];
   
  const int buf_width = xf::util::lcm<WIN_STRM, WOUT_STRM>::value;
  const int num_in    = buf_width/WIN_STRM;
  const int num_out   = buf_width/WOUT_STRM;

  ap_uint<buf_width> buff;
  std::cout<<std::dec<< "WIN_STRM  = "<< WIN_STRM <<std::endl;
  std::cout<<std::dec<< "WOUT_STRM = "<< WOUT_STRM <<std::endl;
  std::cout<<std::dec<< "NSTRM     = "<< NSTRM <<std::endl;
  std::cout<<std::dec<< "NS        = "<< NS <<std::endl;
  for(int d=0; d< NS; ++d)  { 
    ap_uint<WIN_STRM> id =d % NSTRM; 
     float data = d*d +0.5;
    data_istrm.write(data);
    e_istrm.write(false);
#if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
    std::cout<< "id = "<< id << "  "<< "data="<<data << std::endl;
#endif
  } 
  e_istrm.write(true);

  test_core_1_n( data_istrm, e_istrm,
                 data_ostrms, e_data_ostrms);
   
   int nerror=0;
   ap_uint<WIN_STRM> last_data=0;
   ap_uint<NSTRM> ends=0;
   ap_uint<NSTRM> ends_flag= ~ends;
   bool first_data=true;
   int k=0;
   int n=0;
   bool exit_flag=false;
   int count=0;
   int tempa= WIN_STRM*NS/WOUT_STRM;
   int tempb= tempa*WOUT_STRM;
   int comp_count= tempb/WIN_STRM;// comp_count<=NS, last data may not equal NS-1 
   int len[NSTRM] ={0};
   int id=0;
   std::cout<<std::dec<< "comp_count="<<comp_count<<std::endl;
  
   for( int kk=0; kk <NSTRM; ++kk) 
      ends[kk] = e_data_ostrms[kk].read();  
  
   while(ends!= ends_flag) {
     for( int kk=0; kk <NSTRM; ++kk) {
       if(ends[kk]==0){
         ends[kk] = e_data_ostrms[kk].read();  
         float data= data_ostrms[kk].read();
         float gld = n*n +0.5;
         n++;
         len[kk]++;
         if( data != gld) {
           nerror=1; 
           std::cout<<std::dec<< "data = "<<data<< "  "<< "gld = "<< gld<<std::endl;
         }
         else {
          #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
           std::cout<<std::dec<< "data = "<<data<< "  "<< "gld = "<< gld<<std::endl;
           #endif
         }
       }
     } // for kk
   } // while
     // check the data 
     if (n != NS)
       nerror=1;
   for(int id=0; id< NSTRM;id++)
     std::cout<<std::dec<< "the length of stream_"<< id <<": " << len[id] << std::endl;
   if (nerror) 
   {
        std::cout << "\nFAIL: " << nerror << "the order is wrong.\n";
   } else
   {
        std::cout << "\nPASS: no error found.\n";
   }
   return nerror;
}

int main()
{
   return test_1_n(); 
}


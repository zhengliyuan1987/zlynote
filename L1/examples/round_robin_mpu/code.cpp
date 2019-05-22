

#include <vector>
#include <iostream>
#include <stdlib.h>
#include "code.h"

/*
//#include "xf_utils_hw/stream_one_to_n/round_robin.h"
//#include "xf_utils_hw/stream_n_to_one/round_robin.h"
//#include "xf_utils_hw/stream_one_to_n.h"
//#include "xf_utils_hw/stream_n_to_one.h"

#define WIN_STRM  512 
#define WOUT_STRM 64
#define NS       (1024*8)
#define NSTRM    8
*/

ap_uint<WOUT_STRM> compute(int d) {
   ap_uint<WOUT_STRM> nd = d;
/*   int id = d % NSTRM % 4;
//   int id = d % NSTRM +1;
   if ( id ==0 )
     nd *= nd ;
   else if( id ==1 )
     nd += nd;
   else if( id ==2 )
     nd /= 2;
*/
//   nd = nd * id;
   return nd;
};

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
      ap_uint<WOUT_STRM> gld = compute(count);
      if (count <= comp_count && d != gld) {
        nerror=1;
        std::cout<< "erro: "<<"c="<<count<<", gld="<< gld <<", "<< "current data="<<d <<std::endl;
       } 
     count++;
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

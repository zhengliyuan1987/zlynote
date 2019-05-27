//#ifndef __SYNTHESIS__
#include <cstring>
#include <iostream>
#include <stdlib.h>
//#endif

#include "xf_utils_hw/axi_to_stream.h"
#include "xf_utils_hw/stream_to_axi.h"
#include "code.h"
#define AXI_WIDTH     512
#define BURST_LENTH   32
#define SCAL_AXI      (AXI_WIDTH/8)
#define DATA_NUM      4096
#define DATA_LEN      4096
#define STRM_WIDTH    512
typedef ap_uint<STRM_WIDTH> TYPE_Strm;
const int DDR_DEPTH = (DATA_NUM / SCAL_AXI);

// ------------------------------------------------------------
// top functions for general data
void top_core(ap_uint<AXI_WIDTH>* rbuf,
              ap_uint<AXI_WIDTH>* bk_buf,
             //   hls::stream<TYPE_Strm>& ostrm,
             //   hls::stream<bool>& e_ostrm,
              const int len,
              const int offset) {
#pragma HLS INTERFACE m_axi port = rbuf depth = DDR_DEPTH offset = \
    slave bundle = gmem_in0 latency = 8 num_read_outstanding =     \
        32 max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = rbuf bundle = control


#pragma HLS INTERFACE m_axi port = bk_buf depth = DDR_DEPTH offset = \
    slave bundle = gmem_out1 latency = 8 num_write_outstanding =     \
        32 max_write_burst_length = 32

#pragma HLS INTERFACE s_axilite port = bk_buf bundle = control


#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS dataflow
    hls::stream<TYPE_Strm> axi_strm;
#pragma HLS stream variable = axi_strm depth = 8              
    hls::stream<bool> e_axi_strm;
#pragma HLS stream variable = e_axi_strm depth = 8              

    hls::stream<TYPE_Strm> ostrm;
#pragma HLS stream variable = ostrm depth = 8              
    hls::stream<bool> e_ostrm;
#pragma HLS stream variable = e_ostrm depth = 8              
  
  
  //xf::common::utils_hw::axi_to_char_stream<BURST_LENTH>(
  xf::common::utils_hw::axi_to_stream<BURST_LENTH, AXI_WIDTH,TYPE_Strm>(
      rbuf, axi_strm, e_axi_strm, len, offset);

  test_core(axi_strm,e_axi_strm,ostrm,e_ostrm);

  xf::common::utils_hw::stream_to_axi<BURST_LENTH,AXI_WIDTH,STRM_WIDTH >(
      bk_buf,ostrm, e_ostrm);
}

#ifndef __SYNTHESIS_

ap_uint<8> rand_t() {
   ap_uint<8> c = rand() % 256;
   return c; 

}


void gen_data(char* data, const int len) {

   for(int i=0; i< len ;++i)
     data[i]=rand_t();

}

int check_data( char* data, char *res, const int len) {
//  std::cout<<"test data:";
  for(int i=0; i< len; ++i) {
//   std::cout<< res[i];
   if( data[i]!=res[i])
     return 1;
  }
  
  std::cout<<std::endl<< "--------------------"<<std::endl;
  return 0;

}

// ------------------------------------------------------------

int main( ) {
  char* data_ddr = (char*)malloc(DATA_LEN * sizeof(char));
  char* res_ddr  = (char*)malloc(DATA_LEN * sizeof(char));
//    char data_ddr[DATA_LEN]={0};
//    char res_ddr[DATA_LEN]={0};

  //generate data
  int offset    = 0;
  const int len = DATA_LEN;
  gen_data(data_ddr,len);

  // core
  top_core(
        (ap_uint<AXI_WIDTH>*) data_ddr,
        (ap_uint<AXI_WIDTH>*) res_ddr,
         DATA_LEN/SCAL_AXI, offset);

//  check result 
  int err= check_data( data_ddr, res_ddr, len);
  if (err==0)
    std::cout<<"********PASS*********"<<std::endl; 
  else
    std::cout<<"********FAIL*********"<<std::endl; 
  free(data_ddr);
  free(res_ddr);
  return err;
}

#endif

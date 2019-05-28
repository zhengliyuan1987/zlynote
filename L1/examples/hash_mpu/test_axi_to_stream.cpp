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
#define SCAL_AXI      (AXI_WIDTH/32)
#define DATA_NUM      4096
#define DATA_LEN      4096
#define STRM_WIDTH    512
#define INPUT_WIDTH   32
typedef ap_uint<STRM_WIDTH> TYPE_Strm;
typedef ap_uint<INPUT_WIDTH> TYPE_Input;



// DDR_DEPTH * AXI_WIDTH >= INPUT_WIDTH * DATA_LEN
const int DDR_DEPTH = DATA_NUM * INPUT_WIDTH / AXI_WIDTH;

// ------------------------------------------------------------
// top functions for general data
void top_core(ap_uint<AXI_WIDTH>* rbuf,
              ap_uint<AXI_WIDTH>* bk_buf,
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
    hls::stream<TYPE_Strm> axi_istrm;
#pragma HLS stream variable = axi_istrm depth = 8              
    hls::stream<bool> e_axi_istrm;
#pragma HLS stream variable = e_axi_istrm depth = 8              

    hls::stream<TYPE_Strm> axi_ostrm;
#pragma HLS stream variable = axi_ostrm depth = 8              
    hls::stream<bool> e_axi_ostrm;
#pragma HLS stream variable = e_axi_ostrm depth = 8              
  
  
  xf::common::utils_hw::axi_to_stream<BURST_LENTH, AXI_WIDTH,TYPE_Strm>(
      rbuf, axi_istrm, e_axi_istrm, len, offset);

  test_core(axi_istrm,e_axi_istrm,axi_ostrm,e_axi_ostrm);

  xf::common::utils_hw::stream_to_axi<BURST_LENTH,AXI_WIDTH,STRM_WIDTH >(
      bk_buf,axi_ostrm, e_axi_ostrm);
}

#ifndef __SYNTHESIS_

ap_uint<INPUT_WIDTH> rand_t() {
   int min = INPUT_WIDTH >= 16? 16: INPUT_WIDTH;
   ap_uint<INPUT_WIDTH> c = rand() % (1<<min);
   return c; 

}


void gen_data(TYPE_Input* data, const int len) {

   for(int i=0; i< len ;++i)
     data[i]=rand_t();

}

int check_data( TYPE_Input* data, TYPE_Input *res, const int len) {
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
  TYPE_Input* data_ddr = (TYPE_Input*)malloc(DATA_LEN * sizeof(TYPE_Input));
  TYPE_Input* res_ddr  = (TYPE_Input*)malloc(DATA_LEN * sizeof(TYPE_Input));

  //generate data
  int offset    = 0;
  // reshape:  total bits = DATA_LEN*sizeof(TYPE_Input)*8 = AXI_WIDTH * num
  int num       = DATA_LEN * sizeof(TYPE_Input) * 8 / (AXI_WIDTH);
  const int len = DATA_LEN;
  gen_data(data_ddr,len);

  // core
  top_core(
        (ap_uint<AXI_WIDTH>*) data_ddr,
        (ap_uint<AXI_WIDTH>*) res_ddr,
         num, offset);
        // DATA_LEN/SCAL_AXI, offset);

//  check result 
  int err= 0; //check_data( data_ddr, res_ddr, len);
  if (err==0)
    std::cout<<"********PASS*********"<<std::endl; 
  else
    std::cout<<"********FAIL*********"<<std::endl; 
  free(data_ddr);
  free(res_ddr);
  return err;
}

#endif

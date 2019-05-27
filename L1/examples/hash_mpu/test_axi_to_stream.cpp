//#ifndef __SYNTHESIS__
#include <cstring>
#include <iostream>
#include <stdlib.h>
//#endif

#include "xf_utils_hw/axi_to_stream.h"
#include "xf_utils_hw/stream_to_axi.h"
#include "code.h"
#define AXI_WIDTH (512)
#define BURST_LENTH (32)
#define SCAL_AXI (16)
#define DATA_NUM (5120)
#define LEN_BYCHAR (4799)

#define DATA_LEN 4096
// just for the l_discount_few.bin test
//#define DATA_NUM      (1)
//#define LEN_BYCHAR    (4)

#define STRM_WIDTH (512)
typedef ap_uint<512> TYPE_Strm;
const int DDR_DEPTH = (DATA_NUM / SCAL_AXI);

// ------------------------------------------------------------
// top functions for aligned data
void top_align_axi_to_stream(ap_uint<AXI_WIDTH>* rbuf,
                             hls::stream<TYPE_Strm>& ostrm,
                             hls::stream<bool>& e_ostrm,
                             const int num,
                             const int offset_num) {
#pragma HLS INTERFACE m_axi port = rbuf depth = DDR_DEPTH offset = \
    slave bundle = gmem_in1 latency = 8 num_read_outstanding =     \
        32 max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = rbuf bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#ifndef __SYNTHESIS__
  if (AXI_WIDTH < 8 * sizeof(TYPE_Strm))
    std::cout << "WARNING:this function is for AXI width is multiple of the "
                 "align data on ddr"
              << std::endl;
#endif
  xf::common::utils_hw::axi_to_stream<BURST_LENTH>(
      rbuf, ostrm, e_ostrm, num, offset_num);
}

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
    slave bundle = gmem_out1 latency = 8 num_read_outstanding =     \
        32 max_write_burst_length = 32

#pragma HLS INTERFACE s_axilite port = bk_buf bundle = control


#pragma HLS INTERFACE s_axilite port = return bundle = control

    hls::stream<TYPE_Strm> axi_strm;
#pragma HLS stream variable = axi_strm depth = 8              
    hls::stream<bool> e_axi_strm;
#pragma HLS stream variable = e_axi_strm depth = 8              

    hls::stream<TYPE_Strm> ostrm;
#pragma HLS stream variable = ostrm depth = 8              
    hls::stream<bool> e_ostrm;
#pragma HLS stream variable = e_ostrm depth = 8              
  
  
  xf::common::utils_hw::axi_to_char_stream<BURST_LENTH>(
      rbuf, axi_strm, e_axi_strm, len, offset);

  test_core(axi_strm,e_axi_strm,ostrm,e_ostrm);

  xf::common::utils_hw::stream_to_axi<BURST_LENTH,AXI_WIDTH,STRM_WIDTH >(
      bk_buf,ostrm, e_ostrm);
}


ap_uint<8> rand_t() {
   ap_uint<8> c = rand() % 256;
   return c; 

}


void gen_data(char* data, const int len) {

   for(int i=0; i< len ;++i)
     data[i]=rand_t();

}

int check_data( char* data, char *res, const int len) {
  for(int i=0; i< len; ++i) {
   if( data[i]!=res[i])
     return 1;
  }
  return 0;

}

// ------------------------------------------------------------

int main( ) {
  char* data_ddr = (char*)malloc(DATA_LEN * sizeof(char));
  char* res_ddr = (char*)malloc(DATA_LEN * sizeof(char));

///  hls::stream<TYPE_Strm> ostrm;
//  hls::stream<bool> e_ostrm;
  int offset=0;
  //const int len = LEN_BYCHAR;
  const int len = DATA_LEN;
  gen_data(data_ddr,len);

  top_core(
        (ap_uint<AXI_WIDTH>*) data_ddr,
        (ap_uint<AXI_WIDTH>*) res_ddr,
         DATA_LEN, offset);

//  check result 
  int err= check_data( data_ddr, res_ddr, len);
  
  free(data_ddr);
  free(res_ddr);
  return err;
}

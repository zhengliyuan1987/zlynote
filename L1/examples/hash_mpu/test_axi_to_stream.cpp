#ifndef __SYNTHESIS__
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#endif

#include "xf_utils_hw/axi_to_stream.h"
#include "code.h"
#define AXI_WIDTH (128)
#define BURST_LENTH (32)
#define SCAL_AXI (4)
#define DATA_NUM (5120)
#define LEN_BYCHAR (4799)
// just for the l_discount_few.bin test
//#define DATA_NUM      (1)
//#define LEN_BYCHAR    (4)

#define STRM_WIDTH (32)
typedef ap_uint<32> TYPE_Strm;
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
    slave bundle = gmem_in1 latency = 8 num_read_outstanding =     \
        32 max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = rbuf bundle = control


#pragma HLS INTERFACE m_axi port = bk_buf depth = DDR_DEPTH offset = \
    slave bundle = gmem_in1 latency = 8 num_read_outstanding =     \
        32 max_write_burst_length = 32

#pragma HLS INTERFACE s_axilite port = bk_buf bundle = control


#pragma HLS INTERFACE s_axilite port = return bundle = control

    hls::stream<TYPE_Strm>& ostrm,
#pragma HLS stream variable = ostrms depth = 8              
    hls::stream<bool>& e_ostrm,
#pragma HLS stream variable = ostrms depth = 8              
  
    hls::stream<TYPE_Strm> axi_strm,
#pragma HLS stream variable = axi_strms depth = 8              
    hls::stream<bool> e_axi_strm,
#pragma HLS stream variable = axi_strms depth = 8              
  
  xf::common::utils_hw::axi_to_char_stream<BURST_LENTH>(
      rbuf, axi_strm, e_axi_strm, len, offset);

  test_core(axi_strm,e_axi_strm,ostrm,ostrm);

  xf::common::utils_hw::stream_to_axi<BURST_LENTH>(
      axi_strm, e_axi_strm,bk_buf);
}


ap_uint<8> rand_t() {
   ap_uint<8> c = rand() % 256;
   return c; 

}


void gen_data(char* data, const int len) {

   for(int i=0; i< len ;++i)
     data[i]=rand_t();

}

// ------------------------------------------------------------

int main( ) {
  char* data_ddr = (char*)malloc(DATA_LEN_CHAR * sizeof(char));
  char* res_ddr = (char*)malloc(DATA_LEN_CHAR * sizeof(char));

///  hls::stream<TYPE_Strm> ostrm;
//  hls::stream<bool> e_ostrm;
  int err;
  const int len = LEN_BYCHAR;
  gen_data(data,len);

  top_core(
        (ap_uint<AXI_WIDTH>*) data_ddr,
        (ap_uint<AXI_WIDTH>*) res_ddr,
         DATA_NUM, offset);

//  check result 

  free(dataInDDR);
}
#endif

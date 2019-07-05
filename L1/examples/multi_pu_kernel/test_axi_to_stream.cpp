#include <cstring>
#include <iostream>
#include <stdlib.h>

#include "xf_utils_hw/axi_to_stream.hpp"
#include "xf_utils_hw/stream_to_axi.hpp"
#include "code.hpp"
#define W_AXI         (W_STRM*2)
#define BURST_LENTH   32
#define DATA_LEN      (4096*16)
#define W_DATA        W_PU

// the type of input and output data
typedef ap_uint<W_DATA> t_data;
// the type of inner stream
typedef ap_uint<W_STRM> t_strm;

// the depth of axi port
// It must meet  DDR_DEPTH * W_AXI >= W_DATA * DATA_LEN
const int DDR_DEPTH = DATA_LEN * W_DATA / W_AXI;

// ------------------------------------------------------------
// top functions
/**
 * @brief Update data
 * A few of data are packeged to a wide width data which is tranferred by axi-port. Extract and update each data from the wide width data.
 * For example, 8 32-bit data are combined to a 256-bit data. Each 32-bit data is updated and output in the same formart as input.
 * Here, each W_AXI bits data in in_buf includes multiple data(ap_uint<W_DATA>) which will be updated.
 *
 * @param in_buf the input buffer
 * @param out_buf the output buffer
 * @param len the number of input data in in_buf 
 *
 */
void top_core(ap_uint<W_AXI>* in_buf,
              ap_uint<W_AXI>* out_buf,
              const int len) {
#pragma HLS INTERFACE m_axi port = in_buf depth = DDR_DEPTH offset = \
    slave bundle = gmem_in0 latency = 8 num_read_outstanding =     \
        32 max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = in_buf bundle = control

#pragma HLS INTERFACE m_axi port = out_buf depth = DDR_DEPTH offset = \
    slave bundle = gmem_out1 latency = 8 num_write_outstanding =     \
        32 max_write_burst_length = 32

#pragma HLS INTERFACE s_axilite port = out_buf bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS dataflow
    hls::stream<t_strm> axi_istrm;
#pragma HLS stream variable = axi_istrm depth = 8              
    hls::stream<bool> e_axi_istrm;
#pragma HLS stream variable = e_axi_istrm depth = 8              

    hls::stream<t_strm> axi_ostrm;
#pragma HLS stream variable = axi_ostrm depth = 8              
    hls::stream<bool> e_axi_ostrm;
#pragma HLS stream variable = e_axi_ostrm depth = 8              

/*
 ----------------         --------------------------------------------------------         -------------------
|                |       |                                                        |       |                   |
|                |       |                                                        |       |                   |
| axi to stream  |  -->  | stream to n streams   --> MPU  ---> n streams to one   |  -->  |  stream to axi    |
|                |       |                                                        |       |                   |
|                |       |                                                        |       |                   |
 ----------------         ---------------------------------------------------------        -------------------
*/

  
// axi --> stream --> compute  --> stream --> axi
// in_buf        axi_port     inner_stream   axi_port   out_buf
// W_DATA ------> W_AXI ------> W_STRM -----> W_AXI ---> W_DATA

// axi to stream
// in_buf --> axi_istrm
  xf::common::utils_hw::axi_to_stream<BURST_LENTH, W_AXI,t_strm>(
      in_buf, axi_istrm, e_axi_istrm, len, 0);

// compute by mutiple process uinits
// axi_istrm --> axi_ostrm
  update_mpu(axi_istrm,e_axi_istrm,axi_ostrm,e_axi_ostrm);

// stream to axi
// axi_ostrm --> out_buf
  xf::common::utils_hw::stream_to_axi<BURST_LENTH,W_AXI,W_STRM >(
      out_buf,axi_ostrm, e_axi_ostrm);
}

// ------------------------------------------------------------

#ifndef __SYNTHESIS_
// random data
ap_uint<W_DATA> rand_t() {
   int min = W_DATA >= 16? 16: W_DATA;
   ap_uint<W_DATA> c = rand() % (1<<min);
   return c; 

}

/**
 * @brief generate test data
 * @param data_buf stored data
 * @param num  the number of generated data 
 */
void gen_data(t_data* data_buf, const int num) {

   for(int i=0; i< num ;++i)
     data_buf[i]=rand_t();

}
/**
 * @brief check the result
 * @param data_buf  original data
 * @param res_buf   updated data
 * @param num  the number of checking data
 * @return 0-pass, 1-fail 
 */
int check_data( t_data* data_buf, t_data *res_buf, const int num) {
 // check the result by their sums because distribution on load balance is NOT order-preserving.
  int sum_gld = 0;
  int sum_res = 0;
  for(int i=0; i< num; ++i) {
     t_data new_data = update_data(data_buf[i]);
     sum_gld += calculate(new_data);
     sum_res += calculate(res_buf[i]);
  }
  return (sum_gld != sum_res);
}


int main( ) {
  t_data* data_ddr = (t_data*)malloc(DATA_LEN * sizeof(t_data));
  t_data* res_ddr  = (t_data*)malloc(DATA_LEN * sizeof(t_data));

  // reshape:  total bits = DATA_LEN*sizeof(t_data)*8 = W_STRM * num
  int num       = DATA_LEN * sizeof(t_data) * 8 / (W_STRM);
  const int len = DATA_LEN;
  //generate data
  gen_data(data_ddr,len);
  std::cout << " W_AXI  = " << W_AXI << "  num = "<< num << std::endl; 
  std::cout << " W_DATA = " << W_DATA <<  "  len = " << len  << std::endl; 
  // core
  top_core(
        (ap_uint<W_AXI>*) data_ddr,
        (ap_uint<W_AXI>*) res_ddr,
         num);

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

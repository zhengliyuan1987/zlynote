
#include "xf_utils_hw/stream_one_to_n.h"
#include "xf_utils_hw/stream_n_to_one.h"

#define W_STRM  512 
#define W_PU    64
#define NS      (8*1024)
#define NPU     8

// a duplicate of input stream, double each data as output
void process_core(
                   hls::stream<ap_uint<W_PU> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<W_PU> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{
  bool last= e_c_istrm.read(); 
  while(!last) {
#pragma HLS pipeline II=1
            ap_uint<W_PU> d  = c_istrm.read();
            ap_uint<W_PU> od = d<<1;
            c_ostrm.write(od);
            e_c_ostrm.write(false);
            last = e_c_istrm.read();
   } //while
   e_c_ostrm.write(true);
}
// NPU PUs work in parallel
void  process_mpu( 
                   hls::stream<ap_uint<W_PU> > c_istrms[NPU],
                   hls::stream<bool> e_c_istrms[NPU],
                   hls::stream<ap_uint<W_PU> > c_ostrms[NPU],
                   hls::stream<bool> e_c_ostrms[NPU])
{
#pragma dataflow
 for( int i=0; i< NPU; ++i) {
     #pragma HLS unroll
      process_core (
                    c_istrms[i],
                    e_c_istrms[i],
                    c_ostrms[i],
                    e_c_ostrms[i]);
  }
}

void round_robin_mpu(hls::stream<ap_uint<W_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<W_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
/*
 * One input stream(istrm) is splitted to multitple streams, and each services a PU.
 * All output streams from PUs are merged to one stream(ostrm).
 * For example, there are 8 PUs, like this:
 *
 *              split           merge
 *              1-->8           8-->1 
 *
 *                |----> PU0 ---->| 
 *                |               |
 *                |----> PU1 ---->|
 *                |               |
 *                |----> PU2 ---->|
 *                |               |
 *                |----> PU3 ---->|
 * istrm  ----->  |               |-----> ostrm
 *                |----> PU4 ---->|
 *                |               |
 *                |----> PU5 ---->|
 *                |               |
 *                |----> PU6 ---->|
 *                |               |
 *                |----> PU7 ---->|
 *
 */

 /*       one to n                     PUs                   n to one 
 * istrm ---------> data_inner_strms -------> new_data_strms ----------> ostrms
 *
 */

#pragma HLS dataflow
     hls::stream<ap_uint<W_PU> > data_inner_strms[NPU];
#pragma HLS stream variable = data_inner_strms depth = 8              
     hls::stream<bool> e_data_inner_strms[NPU];
#pragma HLS stream variable = e_data_inner_strms depth = 8              

     hls::stream<ap_uint<W_PU> > new_data_strms[NPU];
#pragma HLS stream variable = new_data_strms depth = 8              
     hls::stream<bool> e_new_data_strms[NPU];
#pragma HLS stream variable = e_new_data_strms depth = 8              
 
 xf::common::utils_hw::stream_one_to_n<W_STRM, W_PU,NPU>(
                         istrm,  e_istrm,
                         data_inner_strms, e_data_inner_strms,
                         xf::common::utils_hw::round_robin_t());
                       

 process_mpu( data_inner_strms, e_data_inner_strms,
                new_data_strms,   e_new_data_strms);

 
 xf::common::utils_hw::stream_n_to_one<W_PU, W_STRM,NPU>(
                        new_data_strms, e_new_data_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::round_robin_t());
}



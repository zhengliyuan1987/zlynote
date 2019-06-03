
#include "xf_utils_hw/stream_one_to_n.h"
#include "xf_utils_hw/stream_n_to_one.h"
#include "xf_utils_hw/stream_dup.h"

#define W_STRM   32
#define W_TAG    3
#define NTAG     (1<<W_TAG) 
#define NS       8*1024
/**
 * * @brief a duplicate of input stream, updata each data as output
 * @param c_istrm input stream
 * @param e_c_istrm end flag for input stream
 * @param c_ostrm output stream
 * @param e_c_ostrm end flag for output stream
 *
 */
void process_core(
                   hls::stream<ap_uint<W_STRM> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<W_STRM> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{
  bool last= e_c_istrm.read(); 
  while(!last) {
#pragma HLS pipeline II=1
    ap_uint<W_STRM> d  = c_istrm.read();
    ap_uint<W_STRM> od = d+1;
    c_ostrm.write(od);
    e_c_ostrm.write(false);
    last = e_c_istrm.read();
  } //while
  e_c_ostrm.write(true);
}

/**
 * @brief Multiple  PUs work in parallel
 *
 * @param c_istrms input streams
 * @param e_c_istrms end flag for input streams
 * @param c_ostrms output stream
 * @param e_c_ostrms end flag for output streams
 *
 */
void  process_mpu( 
                   hls::stream<ap_uint<W_STRM> > c_istrms[NTAG],
                   hls::stream<bool> e_c_istrms[NTAG],
                   hls::stream<ap_uint<W_STRM> > c_ostrms[NTAG],
                   hls::stream<bool> e_c_ostrms[NTAG])
{
#pragma HLS dataflow
 for( int i=0; i< NTAG; ++i) {
#pragma HLS unroll
   process_core (
                 c_istrms[i],
                 e_c_istrms[i],
                 c_ostrms[i],
                 e_c_ostrms[i]);
  }
}

/**
 * @brief Simutlate that a big task is coumputed by Mutiple Process Units.   
 * Assume each input data has a tag which stands for the index of PUs that the tag-th PU will process the data.
 *
 * @param istrm input stream
 * @param e_istrm end flag for input stream
 * @param tg_strms tag streams, tg_strms[0] for one to n,and tg_strms[1] for n to one
 * @param e_tg_istrms end flag for tag streams
 * @param ostrm input stream
 * @param e_ostrm end flag for output stream
 */
void test_core(hls::stream<ap_uint<W_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<W_TAG> > tg_strms[2],
                   hls::stream<bool> e_tg_strms[2],
                   hls::stream<ap_uint<W_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
/*
 * The data from input stream are distributed to different PUs according their tag(tg_strms[0]).
 * The data from PUs are collected to one stream according their new tag(tg_strms[1]).
 * All output streams from PUs are merged to one stream(ostrm).
 * For example, there are 8 PUs, like this:
 *
 *             dispatch        collect
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
     hls::stream<ap_uint<W_STRM> > data_inner_strms[NTAG];
#pragma HLS stream variable = data_inner_strms depth = 8              
     hls::stream<bool> e_data_inner_strms[NTAG];
#pragma HLS stream variable = e_data_inner_strms depth = 8              
    hls::stream<ap_uint<W_STRM> > new_data_strms[NTAG];
#pragma HLS stream variable = new_data_strms depth = 8              
     hls::stream<bool> e_new_data_strms[NTAG];
#pragma HLS stream variable = e_new_data_strms depth = 8              

 xf::common::utils_hw::stream_one_to_n<W_STRM,W_TAG>(
                         istrm,  e_istrm,
                         tg_strms[0], e_tg_strms[0],
                         data_inner_strms, e_data_inner_strms,
                         xf::common::utils_hw::tag_select_t());

 process_mpu( data_inner_strms, e_data_inner_strms,
                new_data_strms,   e_new_data_strms);
 
 xf::common::utils_hw::stream_n_to_one<W_STRM, W_TAG>(
                   //     data_inner_strms, e_data_inner_strms,
                        new_data_strms, e_new_data_strms,
                        tg_strms[1], e_tg_strms[1],
                        ostrm, e_ostrm,
                        xf::common::utils_hw::tag_select_t());
}


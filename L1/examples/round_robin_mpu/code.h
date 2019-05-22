
#include "xf_utils_hw/stream_one_to_n.h"
#include "xf_utils_hw/stream_n_to_one.h"

#define WIN_STRM  512 
#define WOUT_STRM 16
#define NS       (1024*2*2)
#define NSTRM    32

// a duplicate of input stream

void process_core(
            //       int op,
                   hls::stream<ap_uint<WOUT_STRM> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<WOUT_STRM> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{

    bool last= e_c_istrm.read(); 
    while(!last) {
      #pragma HLS pipeline II=1
              ap_uint<WOUT_STRM> d    = c_istrm.read();
           /*   ap_uint<WOUT_STRM> doub = d << 1;
              ap_uint<WOUT_STRM> pw   = d * d ;
              ap_uint<WOUT_STRM> half = d>>1;
              ap_uint<WOUT_STRM> od = (op == 0) ? pw   :
                                      (op == 1) ? doub :
                                      (op == 2) ? half :
                                                  d    ;
          */   
              ap_uint<WOUT_STRM> od = d  ;
              c_ostrm.write(od);
              e_c_ostrm.write(false);
              last = e_c_istrm.read();
     } //while
 
     e_c_ostrm.write(true);

}

void  process_mpu( 
                   hls::stream<ap_uint<WOUT_STRM> > c_istrms[NSTRM],
                   hls::stream<bool> e_c_istrms[NSTRM],
                   hls::stream<ap_uint<WOUT_STRM> > c_ostrms[NSTRM],
                   hls::stream<bool> e_c_ostrms[NSTRM])
{

   #pragma dataflow
   for( int i=0; i< NSTRM; ++i) {
      #pragma HLS unroll
       process_core (
          //           i%4,
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);

   }
}

void test_core(hls::stream<ap_uint<WIN_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<WIN_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
#pragma HLS dataflow

     hls::stream<ap_uint<WOUT_STRM> > data_inner_strms[NSTRM];
#pragma HLS stream variable = data_inner_strms depth = 8              
     hls::stream<bool> e_data_inner_strms[NSTRM];
#pragma HLS stream variable = e_data_inner_strms depth = 8              

     hls::stream<ap_uint<WOUT_STRM> > new_data_strms[NSTRM];
#pragma HLS stream variable = new_data_strms depth = 8              
     hls::stream<bool> e_new_data_strms[NSTRM];
#pragma HLS stream variable = e_new_data_strms depth = 8              
 
 xf::common::utils_hw::stream_one_to_n<WIN_STRM, WOUT_STRM,NSTRM>(
                         istrm,  e_istrm,
                         data_inner_strms, e_data_inner_strms,
                         xf::common::utils_hw::round_robin_t());
                       

  process_mpu( data_inner_strms, e_data_inner_strms,
                new_data_strms,   e_new_data_strms);

 
  xf::common::utils_hw::stream_n_to_one<WOUT_STRM, WIN_STRM,NSTRM>(
                        new_data_strms, e_new_data_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::round_robin_t());
}



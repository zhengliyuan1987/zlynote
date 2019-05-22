
#include "xf_utils_hw/stream_one_to_n.h"
#include "xf_utils_hw/stream_n_to_one.h"

#define WIN_STRM  512 
#define WOUT_STRM 16
#define NS       (1024*2*2)
#define NSTRM    32
#define MF    1


void process_core_double (
                   hls::stream<ap_uint<WOUT_STRM> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<WOUT_STRM> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{

    bool last= e_c_istrm.read(); 
    while(!last) {
      #pragma HLS pipeline II=1
        bool em= c_istrm.empty();
        if ( false == em) 
        {  
              ap_uint<WOUT_STRM> d = MF * c_istrm.read();
              c_ostrm.write(d);
              e_c_ostrm.write(false);
              last = e_c_istrm.read();
        }
     } //while
 
     e_c_ostrm.write(true);

}
void process_core_sw (
                   bool f_sw,
                   int sw_l,
                   hls::stream<ap_uint<WOUT_STRM> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<WOUT_STRM> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{

    int c=0;
    bool sw=f_sw;
    bool last= e_c_istrm.read(); 
    while(!last) {
      #pragma HLS pipeline II=1
        bool em= c_istrm.empty();
        if ( false==sw && false == em) 
        {  
              ap_uint<WOUT_STRM> d = MF * c_istrm.read();
              c_ostrm.write(d);
              e_c_ostrm.write(false);
              last = e_c_istrm.read();
        }
     if ( ++c == sw_l) {
        sw=!sw;
        c=0;
      }
   } //while
   e_c_ostrm.write(true);
}
// case0
void  process_pass( 
                   hls::stream<ap_uint<WOUT_STRM> > c_istrms[NSTRM],
                   hls::stream<bool> e_c_istrms[NSTRM],
                   hls::stream<ap_uint<WOUT_STRM> > c_ostrms[NSTRM],
                   hls::stream<bool> e_c_ostrms[NSTRM])
{

   #pragma dataflow
   for( int i=0; i< NSTRM; ++i) {
      #pragma HLS unroll
       process_core_double (
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
   }
}



void  process_mpu( 
                   hls::stream<ap_uint<WOUT_STRM> > c_istrms[NSTRM],
                   hls::stream<bool> e_c_istrms[NSTRM],
                   hls::stream<ap_uint<WOUT_STRM> > c_ostrms[NSTRM],
                   hls::stream<bool> e_c_ostrms[NSTRM])
{

   #pragma dataflow
   for( int i=0; i< 2; ++i) {
      #pragma HLS unroll
       process_core_double (
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
   }
   for( int i=2; i< NSTRM; ++i) {
      #pragma HLS unroll
      if( i<4)
       process_core_sw (
                     i%4==0,
                     2,
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);

      else if (i<8)
       process_core_sw (
                     i%4==0,
                     4,
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
     else
       process_core_sw (
                     i%4==0,
                     8,
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
                         xf::common::utils_hw::load_balance_t());
                         //xf::common::utils_hw::round_robin_t());
                       

  process_mpu( data_inner_strms, e_data_inner_strms,
                new_data_strms,   e_new_data_strms);


 
  xf::common::utils_hw::stream_n_to_one<WOUT_STRM, WIN_STRM,NSTRM>(
                     //   data_inner_strms, e_data_inner_strms,
                        new_data_strms, e_new_data_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::load_balance_t());
}



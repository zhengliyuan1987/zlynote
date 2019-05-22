
#include "xf_utils_hw/stream_one_to_n.h"
#include "xf_utils_hw/stream_n_to_one.h"

#define W_STRM  512 
#define W_PU 64
#define W_PRC 32
#define W_DSC 32
#define NS       (1024*2*2)
#define NPU    8
#define MF    1

ap_uint<W_PU> update_data( ap_uint<W_STRM> data) {
      #pragma HLS inline
     ap_uint<W_PRC> p = data.range(W_PRC - 1, 0);
     ap_uint<W_DSC> d = data.range(W_PRC + W_DSC - 1, 0);
     ap_uint<W_PU> nd;
     nd.range(W_PRC-1,0)= p *2;
     nd.range(W_DSC+W_PRC-1,W_PRC)= d+2;
    return nd; 
}

ap_uint<W_PU> mult( ap_uint<W_STRM> data) {
      #pragma HLS inline
     ap_uint<W_STRM> p = data.range(W_PRC - 1, 0);
     ap_uint<W_STRM>    d = data.range(W_PRC + W_DSC - 1, 0);
     ap_uint<W_PU> nd= p *d;
    return nd; 
}

void process_core_double (
                   hls::stream<ap_uint<W_PU> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<W_PU> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{

    bool last= e_c_istrm.read(); 
    while(!last) {
      #pragma HLS pipeline II=1
        bool em= c_istrm.empty();
        if ( false == em) 
        {  
              ap_uint<W_PU> d =  c_istrm.read();
              ap_uint<W_PU> nd = update_data(d) ;
              c_ostrm.write(nd);
              e_c_ostrm.write(false);
              last = e_c_istrm.read();
        }
     } //while
 
     e_c_ostrm.write(true);

}
void process_core_sw (
                   bool f_sw,
                   int sw_l,
                   hls::stream<ap_uint<W_PU> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<W_PU> >& c_ostrm,
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
              ap_uint<W_PU> d =  c_istrm.read();
              ap_uint<W_PU> nd = update_data(d) ;
              c_ostrm.write(nd);
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
                   hls::stream<ap_uint<W_PU> > c_istrms[NPU],
                   hls::stream<bool> e_c_istrms[NPU],
                   hls::stream<ap_uint<W_PU> > c_ostrms[NPU],
                   hls::stream<bool> e_c_ostrms[NPU])
{

   #pragma dataflow
   for( int i=0; i< NPU; ++i) {
      #pragma HLS unroll
       process_core_double (
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
   }
}



void  process_mpu( 
                   hls::stream<ap_uint<W_PU> > c_istrms[NPU],
                   hls::stream<bool> e_c_istrms[NPU],
                   hls::stream<ap_uint<W_PU> > c_ostrms[NPU],
                   hls::stream<bool> e_c_ostrms[NPU])
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
   for( int i=2; i< NPU; ++i) {
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


void test_core(hls::stream<ap_uint<W_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<W_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
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
                         xf::common::utils_hw::load_balance_t());
                         //xf::common::utils_hw::round_robin_t());
                       

  process_mpu( data_inner_strms, e_data_inner_strms,
                new_data_strms,   e_new_data_strms);


 
  xf::common::utils_hw::stream_n_to_one<W_PU, W_STRM,NPU>(
                     //   data_inner_strms, e_data_inner_strms,
                        new_data_strms, e_new_data_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::load_balance_t());
}



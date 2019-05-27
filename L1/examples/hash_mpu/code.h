
#include "xf_utils_hw/stream_one_to_n.h"
#include "xf_utils_hw/stream_n_to_one.h"

#define W_STRM  512 
#define W_PU    64
#define W_PRC   32
#define W_DSC   32
#define NS      (1024*2*2)
#define NPU     8
#define HP      1
#define HPU     (NPU/HP)
#define W_HPU   (W_STRM/HP)

    
template <int _WIn, int _WOut, int _NStrm>
void stream_split(hls::stream<ap_uint<_WIn> >& istrm,
                  hls::stream<bool>& e_istrm,
                  hls::stream<ap_uint<_WOut> > ostrms[_NStrm],
                  hls::stream<bool> e_ostrms[_NStrm] ) {
 const int max = _WIn > _WOut * _NStrm ? _WIn : _WOut * _NStrm;
  bool last = e_istrm.read();
  while (!last) {
#pragma HLS pipeline II = 1
    last = e_istrm.read();
    ap_uint<max> data = istrm.read();
    for (int i = 0; i < _NStrm; ++i) {
#pragma HLS unroll
      ap_uint<_WOut> d = data.range((i + 1) * _WOut - 1, i * _WOut);
      ostrms[i].write(d);
      e_ostrms[i].write(false);
    } // for
  } // while
 for (int i = 0; i < _NStrm; ++i) {
#pragma HLS unroll
  e_ostrms[i].write(true);  
 }
}

template<typename TIn , int NSTRM>
void dup_stream(
                    hls::stream<TIn > &istrm,
                    hls::stream<bool> &e_istrm,
                    hls::stream<TIn > ostrms[NSTRM], 
                    hls::stream<bool> e_ostrms[NSTRM] ) {

 while(!e_istrm.read()){

#pragma HLS pipeline II = 1
   TIn d = istrm.read();
   for( int i=0; i< NSTRM; ++i) {
#pragma HLS unroll
     ostrms[i].write(d);
     e_ostrms[i].write(false);
   }
 }
 for( int i=0; i< NSTRM; ++i) {
#pragma HLS unroll
   e_ostrms[i].write(true);
 }
}
template<typename TIn >
void dup_stream(
                    hls::stream<TIn > &istrm,
                    hls::stream<bool> &e_istrm,
                    hls::stream<TIn > &ostrm, 
                    hls::stream<bool> &e_ostrm ) {

 while(!e_istrm.read()){

#pragma HLS pipeline II = 1
   TIn d = istrm.read();
   ostrm.write(d);
   e_ostrm.write(false);
 }
   e_ostrm.write(true);

}


template<int WIN_STRM, int WL >
void collect_stream(
                    hls::stream<ap_uint<WIN_STRM> >& istrm,
                    hls::stream<bool> &e_istrm,
                    hls::stream<ap_uint<WIN_STRM * WL> >& ostrm,
                    hls::stream<bool> &e_ostrm ) {
int c=0;
 ap_uint<WIN_STRM*WL>  bd=0;
 while(!e_istrm.read()){
#pragma HLS pipeline II = 1
   ap_uint<WIN_STRM>  d = istrm.read();
   bd.range( (c+1)*WIN_STRM-1, c*WIN_STRM)=d;
   if( ++c == WL) {
     ostrm.write(bd);
     e_ostrm.write(false);
     c=0;
     bd=0;
   } 
 }
 if (c!=0) { 
     ostrm.write(bd);
     e_ostrm.write(false);
 }
 e_ostrm.write(true);

}

template<int WIN_STRM, int NP, int NSTRM>
void combine_streams(
                    hls::stream<ap_uint<WIN_STRM> > istrms[NP][NSTRM],
                    hls::stream<bool> e_istrms[NP][NSTRM],
                    hls::stream<ap_uint<WIN_STRM> > ostrms[NP*NSTRM], 
                    hls::stream<bool> e_ostrms[NP*NSTRM] ) {
  
 #pragma HLS dataflow
 for(int i = 0;  i < NP;  ++i ) {
   #pragma HLS unroll
    for(int j = 0; j < NSTRM; ++j) {
   #pragma HLS unroll
     dup_stream< ap_uint<WIN_STRM> > ( istrms[i][j],
                                       e_istrms[i][j], 
                                       ostrms[i*NSTRM+j],
                                       e_ostrms[i*NSTRM+j]);
    }
 }

}


// extract the meaningful data from the input data, and updata it.
ap_uint<W_PU> update_data( ap_uint<W_PU> data) {
 #pragma HLS inline
  ap_uint<W_PRC> p = data.range(W_PRC - 1, 0);
  ap_uint<W_DSC> d = data.range(W_PRC + W_DSC - 1, W_PRC);
  ap_uint<W_PU> nd = 0 ;
  nd.range(W_PRC-1,0)= p ;//* 2;
  nd.range(W_DSC+W_PRC-1,W_PRC)= d;// + 2;
  return nd; 
}
// extract the meaningful data from the input data, then calculate.
ap_uint<W_PU> calculate( ap_uint<W_PU> data) {
 #pragma HLS inline
  ap_uint<W_PU> p  = data.range(W_PRC - 1, 0);
  ap_uint<W_PU> d  = data.range(W_PRC + W_DSC - 1, 0);
  ap_uint<W_PU> nd = p * d;
  return nd; 
}

/**
 * @brief update each data as output
 * read and write 
 * @param c_istrm input stream
 * @param e_c_istrm end flag for input stream
 * @param c_ostrm output stream
 * @param e_c_ostrm end flag for output stream
 *
 */
void process_core_pass (
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
/**
 * @brief  update each data as output, but work intermittently. 
 * @param f_sw flag for work first or sleep, work if it is false, sleep if ture.
 * @param prd the period of work
 * @param c_istrm input stream
 * @param e_c_istrm end flag for input stream
 * @param c_ostrm output stream
 * @param e_c_ostrm end flag for output stream
 *
 */
void process_core_intermission (
                   bool f_sw,
                   int prd,
                   hls::stream<ap_uint<W_PU> >& c_istrm,
                   hls::stream<bool>& e_c_istrm,
                   hls::stream<ap_uint<W_PU> >& c_ostrm,
                   hls::stream<bool>& e_c_ostrm)
{
/*
 *****************************************************************
 * for example, an ideal case as
 * when f_sw = true, prd =4
 *
 *    sleep    --   work   --   sleep    --   work     ...  
 *   4 cycles      4 cycles    4 cycles     4 cycles   
 *
 *
 * when f_sw = false, prd =4
 *
 *     work    --   sleep   --   work    --   sleep     ...  
 *   4 cycles      4 cycles    4 cycles     4 cycles   
 ******************************************************************/
  int c=0;
  bool sw=f_sw;
  bool last= e_c_istrm.read(); 
  while(!last) {
 #pragma HLS pipeline II=1
    bool em= c_istrm.empty();
    if ( false==sw && false == em) {
     // work 
      ap_uint<W_PU> d =  c_istrm.read();
      ap_uint<W_PU> nd = update_data(d) ;
      c_ostrm.write(nd);
      e_c_ostrm.write(false);
      last = e_c_istrm.read();
    } else {
     // sleep
    } // if - else
    if ( ++c == prd ) {
      sw=!sw;
      c=0;
    } // if
 } //while
 e_c_ostrm.write(true);
}

/**
 * @brief Multiple  PUs work in parallel
 * Some work and others sleep at the same time.
 * @param c_istrms input streams
 * @param e_c_istrms end flag for input streams
 * @param c_ostrms output stream
 * @param e_c_ostrms end flag for output streams
 *
 */

void  process_mpu( 
                   const int offset,
                   hls::stream<ap_uint<W_PU> > c_istrms[HPU],
                   hls::stream<bool> e_c_istrms[HPU],
                   hls::stream<ap_uint<W_PU> > c_ostrms[HPU],
                   hls::stream<bool> e_c_ostrms[HPU])
{
/*
 * Assume NPU = 16.
 * All PUs work in parellel at an ideal case as belows:
 *  PU0   ------------------------------------
 *  PU1   ------------------------------------
 *  PU2   --  --  --  --  --  --  --  --  -- 
 *  PU3     --  --  --  --  --  --  --  --  --
 *  PU4   ----    ----    ----    ----    ----
 *  PU5       ----    ----    ----    ----    
 *  PU6       ----    ----    ----    ----    
 *  PU7       ----    ----    ----    ----    
 *  PU8   --------        --------        ----
 *  PU9           --------        --------
 *  PU10          --------        --------
 *  PU11          --------        --------
 *  PU12  --------        --------        ----
 *  PU13          --------        --------
 *  PU14          --------        --------
 *  PU15          --------        --------
 *
 * Here, the mark(-) stands for work and blank does sleep.
 * 
 */
   #pragma HLS dataflow
   // PU0 and PU1 are always working.
   for( int i=0; i < 1; ++ i) {
      #pragma HLS unroll
      // int i = k;// + offset;
       process_core_pass (
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
   }
     //The other PUs work at sometimes 
   for( int i=1; i< HPU; ++i) {
      #pragma HLS unroll
      int  k = i+ offset ;
      if( k<4)
       process_core_intermission (
                     k%2==0,
                     2,
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
      else if (k<8)
       process_core_intermission (
                     k%4==0,
                     4,
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
     else
       process_core_intermission (
                     k%4==0,
                     8,
                     c_istrms[i],
                     e_c_istrms[i],
                     c_ostrms[i],
                     e_c_ostrms[i]);
   }
}


/**
 * @brief Simutlate that a big task is coumputed by Mutiple Process Units.   
 * Assume each input data is a package which could be splitted to a few of small width data, and each small data is processed by a Process Uint(PU) 
 * The PUs work at different speeds, so it is important to dispatch data to PUs and collect data from PUs. Here, distribution on load balance is used.
 *
 * @param istrm input stream
 * @param e_istrm end flag for input stream
 * @param ostrm input stream
 * @param e_ostrm end flag for output stream
 **/
void one_core(
                   const int offset,
                   hls::stream<ap_uint<W_STRM/HP> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<W_PU> > ostrms[HPU],
                   hls::stream<bool> e_ostrms[HPU]) {

 /*       one to n                     PUs                   n to one 
 * istrm ---------> data_inner_strms -------> new_data_strms ----------> ostrms
 *
 */

#pragma HLS dataflow
const int pus = NPU/HP;
     hls::stream<ap_uint<W_PU> > data_inner_strms[pus];
#pragma HLS stream variable = data_inner_strms depth = 8              
     hls::stream<bool> e_data_inner_strms[pus];
#pragma HLS stream variable = e_data_inner_strms depth = 8              
   
 
 xf::common::utils_hw::stream_one_to_n<W_STRM/HP, W_PU,NPU/HP>(
                         istrm,  e_istrm,
                         data_inner_strms, e_data_inner_strms,
                         xf::common::utils_hw::load_balance_t());
                       
  process_mpu( offset,data_inner_strms, e_data_inner_strms,
               ostrms,  e_ostrms);
 

}


void test_core(hls::stream<ap_uint<W_STRM> >& istrm,
//void test_core(hls::stream<ap_uint<W_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<W_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {


#pragma HLS dataflow
     hls::stream<ap_uint<W_STRM> > w_istrm;
#pragma HLS stream variable = w_istrm depth = 8              
     hls::stream<bool> e_w_istrm;
#pragma HLS stream variable = e_w_istrm depth = 8              

     hls::stream<ap_uint<W_STRM/HP> > data_inner_strms[HP];
#pragma HLS stream variable = data_inner_strms depth = 8              
     hls::stream<bool> e_data_inner_strms[HP];
#pragma HLS stream variable = e_data_inner_strms depth = 8              

     hls::stream<ap_uint<W_PU> > data_strms[HP][NPU/HP];
#pragma HLS stream variable = data_strms depth = 8   
#pragma HLS ARRAY_PARTITION variable = data_strms dim=1
 
     hls::stream<bool> e_data_strms[HP][NPU/HP];
#pragma HLS stream variable = e_data_strms depth = 8              
#pragma HLS ARRAY_PARTITION variable = e_data_strms dim=1

     hls::stream<ap_uint<W_PU> > inner_strms[NPU];
#pragma HLS stream variable = inner_strms depth = 8              
     hls::stream<bool> e_inner_strms[NPU];
#pragma HLS stream variable = e_inner_strms depth = 8
/*
collect_stream<32,16 >(istrm,e_istrm,
                       w_istrm,e_w_istrm);
*/

 stream_split<W_STRM, W_STRM/HP, HP>(
                         //w_istrm, e_w_istrm,
                         istrm, e_istrm,
                         data_inner_strms, e_data_inner_strms);
 for(int i=0; i< HP; ++i) {
#pragma HLS unroll
  one_core(
           i*HPU, 
           data_inner_strms[i], e_data_inner_strms[i],
           data_strms[i], e_data_strms[i]
          );
  }
  combine_streams< W_PU,HP, NPU/HP > (
             data_strms, e_data_strms,
             inner_strms, e_inner_strms);

  xf::common::utils_hw::stream_n_to_one<W_PU, W_STRM,NPU>(
                        inner_strms, e_inner_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::load_balance_t());
/*
  xf::common::utils_hw::stream_n_to_one<W_P, W_STRM,NPU>(
                        inner_strms, e_inner_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::load_balance_t());
*/
}
void test_core1(hls::stream<ap_uint<W_STRM> >& istrm,
                   hls::stream<bool>& e_istrm,
                   hls::stream<ap_uint<W_STRM> >& ostrm,
                   hls::stream<bool>& e_ostrm) {

/*
 * One input stream(istrm) is splitted to multitple streams, and each services a PU.
 * All output streams from PUs are merged to one stream(ostrm).
 * Here, the speeds of PUs are not same.
 * 
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
                         xf::common::utils_hw::load_balance_t());
                       
  process_mpu(0, data_inner_strms, e_data_inner_strms,
                new_data_strms,   e_new_data_strms);
 
  xf::common::utils_hw::stream_n_to_one<W_PU, W_STRM,NPU>(
                        new_data_strms, e_new_data_strms,
                        ostrm, e_ostrm,
                        xf::common::utils_hw::load_balance_t());

}



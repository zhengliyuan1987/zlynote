#ifndef XF_UTIL_STREAM_N1_RR
#define XF_UTIL_STREAM_N1_RR

#include "xf_util/types.h"
#include "xf_util/enums.h"
#include "xf_util/common.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {
/* @brief stream distribute, in round-robin order from NStrm input streams.
 *
 * @tparam _WInStrm input stream width.
 * @tparam _WOutStrm output stream width.
 * @tparam _NStrm number of input streams.
 *
 * @param istrms input data streams.
 * @param e_istrms end flag streams for input data.
 * @param ostrm output data stream.
 * @param e_ostrm end flag stream.
 * @param _op algorithm selector.
 */
template <int _WInStrm, int _WOutStrm,int _NStrm>
void strm_n_to_one(hls::stream<ap_uint<_WInStrm> > istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<ap_uint<_WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _op);

/* @brief stream distribute, in round-robin order from NStrm input streams.
 *
 * @tparam _TIn the type of input and output stream.
 * @tparam _NStrm number of input streams.
 *
 * @param istrms input data streams.
 * @param e_istrms end flag streams for input data.
 * @param ostrm output data stream.
 * @param e_ostrm end flag stream.
 * @param _op algorithm selector.
 */
template <typename _TIn, int _NStrm>
void strm_n_to_one(hls::stream<_TIn> istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<_TIn>& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _op);

} // level1
} // util
} // xf

// Implementation
// TODO


namespace xf {
namespace util {
namespace level1 {

namespace details {

/* @brief read data in round-robin order from NStrm input streams.
 * combine  _NStrm data(_WInStrm bits) from input streams to one(_NStrm*_NStrm bits)
 * @tparam _WInStrm input stream width.
 * @tparam _NStrm number of input streams.
 *
 * @param istrms input data streams.
 * @param e_istrms end flag streams for input data.
 * @param lef_n the number of available data in last n_in_strm.
 * @param n_in_strm output stream.
 * @param e_n_in_strm end flag stream.
 */
template <int _WInStrm,  int _NStrm>
void strm_n_to_one_read(hls::stream<ap_uint<_WInStrm> > istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<ap_uint<32> > &left_n,
                   hls::stream<ap_uint<_WInStrm*_NStrm> >& n_in_strm,
                   hls::stream<bool>& e_n_in_strm) {
  ap_uint<_NStrm> ends=0;
  ap_uint<_WInStrm*_NStrm> cmb=0;
  for(int id=0; id< _NStrm; ++id) {    
   #pragma HLS unroll
   ends[id] = e_istrms[id].read();
  }
 // read _NStrm data from input streams at the  same time, then output them
  while(ends == 0 ) {
    #pragma HLS pipeline II=1 
      for(int id=0; id< _NStrm; ++id) {    
         #pragma HLS unroll
           ap_uint<_WInStrm> d = istrms[id].read();
           cmb.range((id+1)*_WInStrm-1, id * _WInStrm)=d;           
           ends[id] = e_istrms[id].read();
        }
        #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
           std::cout<< "comb="<<cmb<<std::endl;
         #endif
       n_in_strm.write(cmb);
       e_n_in_strm.write(false);
  }
  // read data from unfinished streams 
  int left=_NStrm;
  for(int id=0; id< _NStrm; ++id) {    
       if(!ends[id]){  
         ap_uint<_WInStrm> d = istrms[id].read();
         cmb.range((id+1)*_WInStrm-1, id * _WInStrm)=d;           
         ends[id] = e_istrms[id].read();
         left--;
        }
  }
 // output the data from last loop  
 if( left < _NStrm)  {
       n_in_strm.write(cmb);
       e_n_in_strm.write(false);
  }
      #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
         std::cout<< "comb="<<cmb<<std::endl;
       #endif
  // how many data are available in the last loop
  left_n.write(left<_NStrm? left:0);
  
  e_n_in_strm.write(true);

}
/* @brief buffer data to solve different input and output width.
 * 
 *input  _WInStrm * _NStrm bit --> output lcm(_WInStrm*_NStrm, WOutStrm) bits
 * @tparam _WInStrm input stream width.
 * @tparam _WOutStrm output stream width.
 * @tparam _NStrm number of input streams.
 *
 * @param n_in_strm input data stream.
 * @param e_n_in_strm end flag stream for input data.
 * @param lef_n the number of available data in last n_in_strm, input port
 * @param lef_b the number of available data in last buf_strm, output port.
 * @param buf_strm output stream.
 * @param e_buf_strm end flag stream.
 */
template <int _WInStrm, int _WOutStrm,int _NStrm>
void strm_n_to_one_collect(
                   hls::stream<ap_uint<_WInStrm*_NStrm> > &n_in_strm,
                   hls::stream<bool> &e_n_in_strm,
                   hls::stream<ap_uint<32> > &left_n,
                   hls::stream<ap_uint<32> > &left_b,
                   hls::stream<ap_uint< lcm<_WInStrm*_NStrm, _WOutStrm>::value> >& buf_strm,
                   hls::stream<bool>& e_buf_strm) {
  const int buf_size  = lcm<_WInStrm*_NStrm, _WOutStrm>::value;
  const int num_in    = buf_size/_WInStrm;
  const int count_in  = num_in/_NStrm;
  int p=0;
  ap_uint<buf_size> buf_a;
  // pos stands for total available bits in buf_a
  int pos=0;
  bool last= e_n_in_strm.read();
  while(!last)  {
    #pragma HLS pipeline II=1 
    int low=pos;
    pos += _NStrm*_WInStrm;
    buf_a.range(pos-1,low)=n_in_strm.read();    
    last = e_n_in_strm.read();
    //output when the buf_a is full
    if(!last && p+1== count_in) {
      e_buf_strm.write(false);
      buf_strm.write(buf_a);
      p   = 0;
      pos = 0;
      #if !defined(__SYNTHESIS__) && XF_UTIL_STRM_1NRR_DEBUG == 1
           std::cout<< "buf_a="<<buf_a<<std::endl;
      #endif
     }
    else
    p++;
  }
    buf_strm.write(buf_a);
    e_buf_strm.write(true); // even if true, the data in buf_strm is available.
 
   int left=left_n.read(); 
   if (left >0)  //left is the number of available data(WInStrm bits) in last data(_NStrm*_WInStrm bits) from n_in_strm
     left_b.write(pos-(_NStrm-left)*_WInStrm);
   else
     left_b.write(pos);
}


/* @brief output data sequentially from input stream with big width.
 * 
 * output buf_size=lcm(_WInStrm*_NStrm, WOutStrm) bits in buf_size/_WOutStrm cycles
 * @tparam _WInStrm input stream width.
 * @tparam _WOutStrm output stream width.
 * @tparam _NStrm number of input streams.
 *
 * @param buf_strm input data stream.
 * @param e_buf_strm end flag stream for input data.
 * @param lef_b the number of available data in last buf_strm, input port.
 * @param ostrm output stream.
 * @param estrm end flag stream.
 */
template <int _WInStrm, int _WOutStrm, int _NStrm>
void strm_n_to_one_distribute(
                   hls::stream<ap_uint< lcm<_WInStrm*_NStrm, _WOutStrm>::value> >& buf_strm,
                   hls::stream<bool> &e_buf_strm,
                   hls::stream<ap_uint<32> > &left_b,
                   hls::stream<ap_uint<_WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
  const int buf_size = lcm<_WInStrm*_NStrm, _WOutStrm>::value;
  const int num_out = buf_size/_WOutStrm;

  ap_uint<buf_size> buf_b;
  int low   = 0;
  int up    = 0;
  int c     = num_out;
  bool last = false; 
  unsigned int up_pos= -1;  
  while(!last) {
   #pragma HLS pipeline II=1
    // read once, output num_out data 
    if(c==num_out) {
        buf_b= buf_strm.read();    
        c=0;
        last= e_buf_strm.read(); 
        //when  e_buf_strm is true, read left_b  
        if (last)
         up_pos=left_b.read(); // up_pos is changed only if the input will end
     }
    // ouput data at almost every cycle 
    if( (c+1)*_WOutStrm< up_pos) { 
       ostrm.write(buf_b.range((c+1)*_WOutStrm-1, c*_WOutStrm));
       e_ostrm.write(false);
     }
      c++;
  } // while
 e_ostrm.write(true);

}

/**
 *  read data from _NStrm input streams and output them to one stream in order of round robin
 *  assume the input and output width( _WInStrm and _WOutStrm) are different.
 */
template <int _WInStrm, int _WOutStrm, int _NStrm>
void strm_n_to_one_round_robin(hls::stream<ap_uint<_WInStrm> > istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<ap_uint<_WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm) {
  
const int buf_size = lcm<_WInStrm*_NStrm, _WOutStrm>::value;
hls::stream<ap_uint<_WInStrm*_NStrm> > n_in_strm;
#pragma HLS stream variable = n_in_strm depth = 8
hls::stream<bool> e_n_in_strm;
#pragma HLS stream variable = e_n_in_strm depth = 8
hls::stream<ap_uint<buf_size> > buf_strm;
#pragma HLS stream variable = buf_strm depth = 8
hls::stream<bool> e_buf_strm;
#pragma HLS stream variable = e_buf_strm depth = 8
hls::stream<ap_uint<32> > left_n; //how many input(_WInStrm bits) are stored in  last data(_NStrm*WInstrm bits) in n_in_strm
hls::stream<ap_uint<32> > left_b; // how many input(_WInStrm bits) are stored in last data(buf_size bits) in buf_strm
 
#pragma HLS dataflow

/*  read data   : read data from input streams, output _NStrm * _WInStrm bits
 *  collect data: buffer  buf_size=lcm<_WInStrm*_NStrm, _WOutStrm> bits and output them.
 *  distribute  : output buf_size/_WOutStrm  data when read buf_size bits once
 *
 * least common mutiple(lcm) is used for solving the difference between  the input width and output width
 *
 * */
 
strm_n_to_one_read < _WInStrm, _NStrm>
                ( istrms,
                  e_istrms,
                  left_n,
                  n_in_strm,
                  e_n_in_strm);

strm_n_to_one_collect < _WInStrm, _WOutStrm, _NStrm>
                (  n_in_strm,
                   e_n_in_strm,
                   left_n,
                   left_b,
                   buf_strm,
                   e_buf_strm);


strm_n_to_one_distribute < _WInStrm,  _WOutStrm, _NStrm>
               ( buf_strm,
                 e_buf_strm,
                 left_b,
                 ostrm,
                 e_ostrm);
}


} // details

template <int _WInStrm, int _WOutStrm, int _NStrm>
void strm_n_to_one(hls::stream<ap_uint<_WInStrm> > istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<ap_uint<_WOutStrm> >& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _op) {
   
    details::strm_n_to_one_round_robin<_WInStrm, _WOutStrm, _NStrm>( istrms,
                                       e_istrms,
                                       ostrm,
                                       e_ostrm);

}


//------------------------------------------------------------------------------//
namespace details {
template <typename _TIn, int _NStrm>
void strm_n_to_one_round_robin_type(hls::stream<_TIn> istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<_TIn>& ostrm,
                   hls::stream<bool>& e_ostrm) {  
 
 int id=0;
 ap_uint<_NStrm> end=0;
 ap_uint<_NStrm> end_flag=~end;
 for(int i=0; i< _NStrm; ++i) {    
   #pragma HLS unroll
   end[id] = e_istrms[i].read();
  }
 // output the data from input in order of round robin 
 while( end != end_flag) {
   if(!end[id]){
      _TIn d = istrms[id].read();
      end[id]=e_istrms[id].read();
      id = (id+1)== _NStrm ? 0 : (id+1);
      ostrm.write(d);
      e_ostrm.write(false);
   } 
}
 e_ostrm.write(true);
}

} // details

template <typename _TIn, int _NStrm>
void strm_n_to_one(hls::stream<_TIn> istrms[_NStrm],
                   hls::stream<bool> e_istrms[_NStrm],
                   hls::stream<_TIn>& ostrm,
                   hls::stream<bool>& e_ostrm,
                   round_robin_t _op) {
    details::strm_n_to_one_round_robin_type<_TIn, _NStrm>( istrms,
                                       e_istrms,
                                       ostrm,
                                       e_ostrm);

}

} // level1
} // util
} // xf
#endif // XF_UTIL_STREAM_N1_RR

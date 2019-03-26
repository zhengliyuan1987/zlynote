#ifndef XF_UTIL_STREAM_N1_TAG
#define XF_UTIL_STREAM_N1_TAG

#include "xf_util/types.h"
#include "xf_util/traits.h"
#include "xf_util/enums.h"

// Forward decl

namespace xf {
namespace util {
namespace level1 {

/*tag based collect, select tag
* @brief strm_n_to_one primitive implements that n streams is mereged  to one stream.
* In this primitive, each tag decides the output order when the n data arrive at same time. 
* The output data in data_ostrms only contains the input data .
*
* @tparam _WInStrm  the width of input data
* @tparam _WTagStrm the width of tag,  pow(2, _WTagStrm) is the number of input streams.
*
* @param data_istrms the input streams
* @param e_data_istrms the end flag of input streams
* @param tag_istrm  the tag stream, streams and data_istrm and tag_istrm are synchronous.
* @param e_tag_istrm the end signal stream, true if data_istrms and tag_istrm are ended.
* @param data_ostrm the output stream. 
* @param e_data_ostrm the end signals of data_ostrm.
* @param _op   algorithm selector
*/
template <int _WInStrm, int _WTagStrm>
void strm_n_to_one(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op);

/*tag based collect, select tag
* @brief strm_n_to_one primitive implements that n streams is mereged  to one stream.
* In this primitive, each tag decides the output order when the n data arrive at same time. 
* The output data in data_ostrms only contains the input data .
*
* @tparam _TIn  the type of input data
* @tparam _WTagStrm the width of tag,  pow(2, _WTagStrm) is the number of input streams.
*
* @param data_istrms the input streams
* @param e_data_istrms the end flag of input streams
* @param tag_istrm  the tag stream, streams and data_istrm and tag_istrm are synchronous.
* @param e_tag_istrm the end signal stream, true if data_istrms and tag_istrm are ended.
* @param data_ostrm the output stream. 
* @param e_data_ostrm the end signals of data_ostrm.
* @param _op   algorithm selector
*/
template <typename _TIn, int _WTagStrm>
void strm_n_to_one(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op);

} // level1
} // utils
} // xf

// Implementation
// TODO


namespace xf {
namespace util {
namespace level1 {
namespace details {

template <int _WInStrm, int _WTagStrm>
void strm_n_to_one_select(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm) {

  const int n=PowerOf2<_WTagStrm>::value;
  ap_uint<n> ends=0;
  ap_uint<n> bak_ends=0;
  bool last_tag  = e_tag_istrm.read();
  for(int i=0; i< n; ++i) {
  #pragma HLS unroll
   ends[i] = e_data_istrms[i].read(); 
  }
  while(!last_tag)
  {
  #pragma HLS pipeline II = 1
    ap_uint<_WTagStrm> tag = tag_istrm.read(); 
   bak_ends=ends; 
   if(!bak_ends[tag]) {
       ap_uint<_WInStrm> data  = data_istrms[tag].read();
       data_ostrm.write(data);
       e_ostrm.write(false);
       ends[tag] = e_data_istrms[tag].read();
    }
    else {
      // stream tag is over. skip or write something ?
     // assert();
    }
    last_tag  = e_tag_istrm.read();
  } // while
  // drop
  for(int i=0; i< n; ++i) {
  #pragma HLS unroll
    while(!ends[i]) {
      ends[i] = e_data_istrms[i].read();
      ap_uint<_WInStrm> data  = data_istrms[i].read();
    } // while 
  } // for
  
  e_ostrm.write(true);

}

} // details
// tag based collect, combine tag
template <int _WInStrm, int _WTagStrm>
void strm_n_to_one(
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> >& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op) {

 details::strm_n_to_one_select<_WInStrm, _WTagStrm>(
                                data_istrms,
                                e_data_istrms,
                                tag_istrm,
                                e_tag_istrm,
                                data_ostrm,
                                e_ostrm ) ;

}
//-------------------------------------------------------------------------------/
namespace details {

template <typename _TIn, int _WTagStrm>
void strm_n_to_one_select_type(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm) {

  const int n=PowerOf2<_WTagStrm>::value;
  ap_uint<n> ends = 0;
  bool last_tag   = e_tag_istrm.read();
  for(int i=0; i< n; ++i) {
  #pragma HLS unroll
   ends[i] = e_data_istrms[i].read(); 
  }
  while(!last_tag)
  {
  #pragma HLS pipeline II = 1
    ap_uint<_WTagStrm> tag = tag_istrm.read(); 
    if(!ends[tag]) {
       _TIn data  = data_istrms[tag].read();
       data_ostrm.write(data);
       e_ostrm.write(false);
       ends[tag] = e_data_istrms[tag].read();
    }
    else {
      // stream tag is over. skip or write something ?
     // assert();
    }
    last_tag  = e_tag_istrm.read();
  } // while
  // drop
  for(int i=0; i< n; ++i) {
  #pragma HLS unroll
    while(!ends[i]) {
      ends[i]    = e_data_istrms[i].read();
      _TIn data  = data_istrms[i].read();
    } // while 
  } // for
  
  e_ostrm.write(true);

}
} // details
// tag based collect, discard tag
template <typename _TIn, int _WTagStrm>
void strm_n_to_one(
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn>& data_ostrm,
    hls::stream<bool>& e_ostrm,
    tag_select_t _op) {

 details::strm_n_to_one_select_type<_TIn, _WTagStrm>(
                                data_istrms,
                                e_data_istrms,
                                tag_istrm,
                                e_tag_istrm,
                                data_ostrm,
                                e_ostrm ) ;

}

} // level1
} // utils
} // xf






#endif // XF_UTIL_STREAM_N1_TAG

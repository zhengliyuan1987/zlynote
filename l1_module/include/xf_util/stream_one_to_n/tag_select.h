#ifndef XF_UTIL_STREAM_1N_TAG_H
#define XF_UTIL_STREAM_1N_TAG_H

#include "xf_util/types.h"
#include "xf_util/enums.h"
#include "xf_util/traits.h"
#include <assert.h>
// Forward decl

namespace xf {
namespace util {
namespace level1 {

/* 
* @brief strm_one_to_n primitive implements that one stream is distributed to n streams.
* In this primitive, the tag is the index of ouput streams. 
* The input data in data_istrms is distributed to  the data_ostrm whose index is tag .
* @tparam _WInStrm  the width of input data
* @tparam _WTagStrm the width of tag,  pow(2, _WTagStrm) is the number of ouput streams.
*
* @param data_istrm the input stream
* @param tag_istrm  the  tag stream,  each tag is the index of output streams and data_istrm and tag_istrm are synchronous.
* @param e_tag_istrm the end signal stream, true if data_istrm and tag_istrm are ended.
* @param data_ostrms the output stream. 
* @param e_data_ostrms the end signals of data_ostrms.
* @param _op  tag lable
*/
template <int _WInStrm, int _WTagStrm>
void strm_one_to_n(
    hls::stream<ap_uint<_WInStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> > data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    tag_select_t _op);

/* 
* @brief strm_one_to_n primitive implements that one stream is distributed to n streams.
* In this primitive, the tag is the index of ouput streams. 
* The input data in data_istrms is distributed to  the data_ostrm whose index is tag .
* @tparam _TIn  the type of input & output data
* @tparam _WTagStrm the width of tag,  pow(2, _WTagStrm) is the number of ouput streams.
*
* @param data_istrm the input stream
* @param tag_istrm  the  tag stream,  each tag is the index of output streams and data_istrm and tag_istrm are synchronous.
* @param e_tag_istrm the end signal stream, true if data_istrm and tag_istrm are ended.
* @param data_ostrms the output stream. 
* @param e_data_ostrms the end signals of data_ostrms.
* @param _op  tag lable
*/
template <typename _TIn, int _WTagStrm>
void strm_one_to_n(
    hls::stream<_TIn>& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn> data_istrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_istrms[PowerOf2<_WTagStrm>::value],
    tag_select_t _op);

} // level1
} // util
} // xf
////////////////////////////////////////////////////////////////////////////

// Implementation

namespace xf {
namespace util {
namespace level1 {
namespace details{

template <int _WInStrm, int _WTagStrm>
void strm_one_to_n_tag_select(
    hls::stream<ap_uint<_WInStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> > data_ostrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_ostrms[PowerOf2<_WTagStrm>::value]){

  bool last_tag    = e_tag_istrm.read();
  bool last_istrm  = e_istrm.read();
  while(!last_tag && !last_istrm) {
  #pragma HLS pipeline II = 1
    ap_uint<_WInStrm> data  = istrm.read();
    ap_uint<_WTagStrm> tag  = tag_istrm.read(); 
    data_ostrms[tag].write(data);
    e_data_ostrms[tag].write(false);
    last_tag    = e_tag_istrm.read();
    last_istrm  = e_istrm.read();
  }
  // drop 
  while(!last_istrm) {
     last_istrm              = e_istrm.read();
     ap_uint<_WInStrm> data  = istrm.read();
  }
  
  assert(last_tag);

  const unsigned int  nstrm = PowerOf2<_WTagStrm>::value;
  for(unsigned int  i=0; i< nstrm; ++i) {
    #pragma HLS unroll
    e_data_ostrms[i].write(true);
  }

} 

} //details

template <int _WInStrm, int _WTagStrm>
void strm_one_to_n(
    hls::stream<ap_uint<_WInStrm> >& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<ap_uint<_WInStrm> > data_ostrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_ostrms[PowerOf2<_WTagStrm>::value],
    tag_select_t _op){

    details::strm_one_to_n_tag_select<_WInStrm, _WTagStrm>( istrm,
   	                        e_istrm,
   	                        tag_istrm,
   	                        e_tag_istrm,
   	                        data_ostrms,
   	                        e_data_ostrms
   	                        );
}
//--------------------------------------------------------------------


namespace details{

template <typename _TIn, int _WTagStrm>
void strm_one_to_n_tag_select_type (
    hls::stream<_TIn>& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn> data_ostrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_ostrms[PowerOf2<_WTagStrm>::value]) {

  bool last_tag    = e_tag_istrm.read();
  bool last_istrm  = e_istrm.read();
  while(!last_tag && !last_istrm) {
  #pragma HLS pipeline II = 1
    _TIn data               = istrm.read();
    ap_uint<_WTagStrm> tag  = tag_istrm.read(); 
    data_ostrms[tag].write(data);
    e_data_ostrms[tag].write(false);
    last_tag    = e_tag_istrm.read();
    last_istrm  = e_istrm.read();
  }
  // drop 
  while(!last_istrm) {
     last_istrm = e_istrm.read();
     _TIn  data = istrm.read();
  }
  
  assert(last_tag);

  const unsigned int  nstrm = PowerOf2<_WTagStrm>::value;
  for(unsigned int  i=0; i< nstrm; ++i) {
    #pragma HLS unroll
    e_data_ostrms[i].write(true);
  }

} 

} // details

template <typename _TIn, int _WTagStrm>
void strm_one_to_n(
    hls::stream<_TIn>& istrm,
    hls::stream<bool>& e_istrm,
    hls::stream<ap_uint<_WTagStrm> >& tag_istrm,
    hls::stream<bool>& e_tag_istrm,
    hls::stream<_TIn> data_ostrms[PowerOf2<_WTagStrm>::value],
    hls::stream<bool> e_data_ostrms[PowerOf2<_WTagStrm>::value],
    tag_select_t _op){
    details::strm_one_to_n_tag_select_type<_TIn, _WTagStrm>( istrm,
   	                        e_istrm,
   	                        tag_istrm,
   	                        e_tag_istrm,
   	                        data_ostrms,
   	                        e_data_ostrms
   	                        );
}

} // level1
} // util
} // xf

#endif // XF_UTIL_STREAM_1N_TAG_H

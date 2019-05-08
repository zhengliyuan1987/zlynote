#ifndef XF_UTILS_HW_STRM_DISCARD_H
#define XF_UTILS_HW_STRM_DISCARD_H

#include "xf_utils_hw/types.h"

/**
 * @file stream_discard.h
 * @brief discard streams.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Discard multiple streams with end flag helper for each.
 */
template <typename _TIn, int _NStrm>
void stream_discard(hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool> e_istrms[_NStrm]); // TODO

/**
 * @brief Discard multiple streams synchronized with one end flag
 */
template <typename _TIn, int _NStrm>
void stream_discard(hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool>& e_istrm); // TODO

/**
 * @brief Discard one stream with its end flag helper.
 */
template <typename _TIn>
void stream_discard(hls::stream<_TIn>& istrm,
                    hls::stream<bool>& e_istrm); // TODO

} // utils_hw
} // common
} // xf

// Implementation

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Discard multiple streams with end flag helper for each.
 */
template <typename _TIn, int _NStrm>
void stream_discard(hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool> e_istrms[_NStrm]) {

  #pragma HLS dataflow
  for( int i = 0;  i < _NStrm ; ++i) {
    #pragma HLS  unroll
    stream_discard<_TIn>( istrms[i],
                          e_istrms[i]); 
  }
}

/**
 * @brief Discard multiple streams synchronized with one end flag
 */
template <typename _TIn, int _NStrm>
void stream_discard(hls::stream<_TIn> istrms[_NStrm],
                    hls::stream<bool>& e_istrm) {

  while(!e_istrm.read()) {
   #pragma HLS pipeline II=1
   for( int i = 0;  i < _NStrm ; ++i) {
    #pragma HLS  unroll
        _TIn d = istrms[i].read(); 
   }
  } 

}

/**
 * @brief Discard one stream with its end flag helper.
 */
template <typename _TIn>
void stream_discard(hls::stream<_TIn>& istrm,
                    hls::stream<bool>& e_istrm) {

     while( !e_istrm.read()) {
      #pragma HLS pipeline II=1
        _TIn d = istrm.read(); 
     }
}

} // namespace utils_hw
} // namespace common
} // namespace xf

#endif // XF_UTILS_HW_STRM_DISCARD_H

#ifndef XF_UTILS_HW_STRM_REORDER_H
#define XF_UTILS_HW_STRM_REORDER_H

#include "xf_utils_hw/types.h"

/**
 * @file stream_reoder.h
 * @brief utility to do window-reorder on a stream.
 *
 * This file is part of XF Hardware Utilities Library.
 */

// Forward decl ===============================================================

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief Window-reorder in a stream.
 *
 * Suppose 8bit RGB values are multiplexed into one stream in R-G-B order,
 * and a process module expects channel values in B-G-R order,
 * then the data needs to be reordered in a window with size 3,
 * and the configuration would be ``2, 1, 0``.
 *
 * It is assumed that _the total number of elements passed through this module
 * is multiple of window-size_. Otherwise, the module may hang in execution.
 *
 * The configuration is load once in one invocation, and reused until the end.
 * Totally ``_WindowSize`` index integers will be read.
 *
 * @tparam _TIn input type.
 * @tparam _WindowSize size of reorder window.
 *
 * @param order_cfg the new order within the window, indexed from 0.
 * @param istrm input data stream.
 * @param e_istrm end flags for input.
 * @param ostrm output data stream.
 * @param e_ostrm end flag for output.
 */
template <typename _TIn, int _WindowSize>
void stream_reorder(hls::stream<int> & order_cfg,

                    hls::stream<_TIn>& istrm,
                    hls::stream<bool>& e_istrm,

                    hls::stream<_TIn>& ostrm,
                    hls::stream<bool>& e_ostrm); // TODO

} // utils_hw
} // common
} // xf

// Implementation =============================================================

namespace xf {
namespace common {
namespace utils_hw {

template <typename _TIn, int _WindowSize>
void stream_reorder(hls::stream<int> & order_cfg,
                    hls::stream<_TIn>& istrm,
                    hls::stream<bool>& e_istrm,
                    hls::stream<_TIn>& ostrm,
                    hls::stream<bool>& e_ostrm) {
/**
 *
 * _WindowSize = 4 
 * cfg           2031
 * order[0:3]    2031    i.e. order[0]=2, order[1]=0, order[2]=3,order[3]=1
 * istrm         abcd efgh ijkl  mnop
 *
 * buff_p[0:3]   abcd abcd ijkl  ijkl
 * op on buff_p   w_p r_p  w_p   r_p
 * buff_q[0:3]        efgh efgh  mnop mnop
 * op on buff_q       w_q  r_q   w_q  r_q
 *
 * ostrm              cadb gehf kilj  ompn
 *
 *in which  w_p = write buff_p, r_p = read buff_p then output each data
 *          w_q = write buff_q, r_q = read buff_q then output each data
 *
 *                    
 */
  int  c   = 0;
  bool sw  = true;
  bool b_d = false; // flag for store input data  already or not yet
  int order[_WindowSize];
  _TIn buff_p[_WindowSize];
  _TIn buff_q[_WindowSize];
  bool last = e_istrm.read();
  // read order_cfg 
 if( !last) { 
  for(int i=0; i < _WindowSize; ++i) {
  #pragma HLS pipeline II=1 
   order[i]  = order_cfg.read();
   buff_p[i] = istrm.read();
   last      = e_istrm.read();
  }
  b_d = true;
 }
 // read new data and output old data at the same time 
 while( !last){
  #pragma HLS pipeline II=1 
  _TIn d = istrm.read();
  last   = e_istrm.read();
   // order[c] is the positin of the c_th output in buff_p/buff_q
  int p = order[c]; 
  _TIn od;
  if(sw) {
     buff_q[c] = d;
     od        = buff_p[p];
  }
  else {
     buff_p[c] = d; 
     od        = buff_q[p];
  }
  ostrm.write(od);
  e_ostrm.write(false); 
  c++;
  if(c== _WindowSize) {
     c  = 0   ;
     sw = !sw ;
   }
 } // while
    
 /*
 * if the input number is multiple of _WindowSize,
 *  c=0 here when above while-loop quits, 
 * */
if( c>0) {
  // even if e_istrm is finished, read istrm until buff_p/buff_q is full (i.e total number is  multiple of _WinSize)
  // this leads to hang if no longer  input data
   for(int i=c; i < _WindowSize; ++i) {
    #pragma HLS pipeline II=1 
    _TIn d = istrm.read();
     // order[c] is the positin of the c_th output in buff_p/buff_q
    int p = order[i]; 
    _TIn od;
    if(sw) {
       buff_q[i] = d;
       od        = buff_p[p];
    }
    else {
       buff_p[i] = d; 
       od        = buff_q[p];
    }
    ostrm.write(od);
    e_ostrm.write(false); 
   }
   sw = !sw;
}
 // if total input number is not 0, output the last _WinSize data from buff_p/buff_q
 if(b_d) {
   for(int i=0; i < _WindowSize; ++i) {
    #pragma HLS pipeline II=1 
     int p = order[i];
     _TIn od = sw ? buff_p[p]: buff_q[p];
     ostrm.write(od);
     e_ostrm.write(false); 
   }
 }
 e_ostrm.write(true); 

}

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_STRM_REORDER_H
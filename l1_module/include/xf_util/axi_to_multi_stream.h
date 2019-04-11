#ifndef XF_UTIL_AXI_TO_MULTI_STRM_H
#define XF_UTIL_AXI_TO_MULTI_STRM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl
#ifndef __SYNTHESIS__
int cnt_test=0;
#endif
const int  NONBLOCK_DEPTH  =  (256);

namespace xf {
namespace util {
namespace level1 {
namespace details{

// ---------------------- details ---------------------------------
template<int _WAxi, int _BurstLen>
void axi_onetype_batch_to_ram(
        const ap_uint<_WAxi>*   rbuf,
        const int               off_axi,
        const int               len,
        int&              		pos,
		ap_uint<_WAxi>  		dat_ram[_BurstLen],
        int&             		len_ram,
        int&             		pos_ram
){
//#pragma HLS INLINE
    if(pos == len)
        return ;
    if(len_ram!=pos_ram)
        return ;
    else {
    	//aligned to read len times
    	const ap_uint<_WAxi> * vec_ptr = rbuf + pos + off_axi;
    	//ap_uint<_WAxi > 		   ram = 0;

    	for(int n=0; n<_BurstLen; n++){
#pragma HLS loop_tripcount min=1 max=1
#pragma HLS PIPELINE II=1
    		dat_ram[n]= vec_ptr[n];
    	}
        if( _BurstLen >= len -pos  ){//All data has been copied
            len_ram = len - pos;
            pos     = len;
        }else {//Full size is copied but still remain one or more
            len_ram = _BurstLen;
            pos    += _BurstLen;
        }
        pos_ram=0;
    }
}

template<int _WAxi, int _BurstLen, typename _TStrm>
bool non_blocking_onetype_ram_to_stream(
        bool                    is_onetype_fnl,
        const int               len,
        const int              	pos,
		ap_uint<_WAxi> 		   	dat_ram[_BurstLen],
        const int              	len_ram,
        int&              		pos_ram,
hls::stream<ap_uint<_WAxi> >&   vec_strm
//		£¬hls::stream<bool>& 		e_ostrm
        )
{

    if(is_onetype_fnl){
        return is_onetype_fnl;
    }
    bool isFinalBlock     = len==pos;
    bool hasDone_blk      = len_ram==pos_ram;
    bool isFull           = false;
    //reorg and split
    ap_uint<_WAxi> vec_reg;

    NON_BLOCKING_LOOP:
    while( isFull==false && hasDone_blk==false ){
#pragma HLS loop_tripcount min=1 max=32  avg=32
#pragma HLS PIPELINE II=1
    	ap_uint<_WAxi> rd = 0;
    	            rd = dat_ram[pos_ram];

    	isFull         = vec_strm.full();
        is_onetype_fnl = (!isFull)&(isFinalBlock)& (((pos_ram+1)==len_ram))?true:false;
        hasDone_blk    = (!isFull)&                (((pos_ram+1)==len_ram))?true:false;
        if(!isFull){
        	vec_strm.write(rd);
#ifndef __SYNTHESIS__
            cnt_test++;
#endif
        }
        (pos_ram)++;
    }//end while

    if(isFull){
        (pos_ram)--;
    }
    return is_onetype_fnl;
}

template<int _WAxi, int _BurstLen, typename _TStrm>
bool axi_batchdata_to_stream(
        const ap_uint<_WAxi>*   rbuf,
        const int               off_axi,
		const int               len,
		int&              		pos,
		ap_uint<_WAxi>          dat_ram[_BurstLen],
        int&              		len_ram,
		int&              		pos_ram,
        bool              		is_onetype_fnl,
hls::stream<ap_uint<_WAxi> >&	vec_strm
		//hls::stream<bool>& 		e_ostrm
){
//#pragma HLS DATAFLOW
	/////////////// load data to local ram  ///////////////
	details::axi_onetype_batch_to_ram<_WAxi, _BurstLen>(
	           rbuf,
			   off_axi,
	           len,
	           pos,
	           dat_ram,
	           len_ram,
	           pos_ram);

	/////////////// nonblocking write one type data form ram to strm  ///////////////
	bool is_fnl = details::non_blocking_onetype_ram_to_stream<_WAxi, _BurstLen, _TStrm >(
        is_onetype_fnl,
        len,
        pos,
        dat_ram,
        len_ram,
        pos_ram,
		vec_strm
        //,e_ostrm
		);
	return is_fnl;

}

template <int _WAxi, int _BstLen, typename _TStrm0, typename _TStrm1, typename _TStrm2>
void read_to_vec_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<ap_uint<_WAxi> >& vec_strm0,
    hls::stream<ap_uint<_WAxi> >& vec_strm1,
    hls::stream<ap_uint<_WAxi> >& vec_strm2,
	const int len    [3],
    const int offset [3]
		  //,int off_ali[3]
){
	const int        numType = 3;
	const int 	   scal_char = _WAxi/8;
	  	  int        len_vec [3];//length for one type vector, by width of Axi
	 	  int        pos_vec [3];//current position for one type vec, by width of Axi
	 	  int        len_ram [3];//length for one type vector loaded in local ram
	 	  int        pos_ram [3];//current position need to be read
	 	  int        off_ali [3];//the first row offset aligned to Axi port by char
	 	  int        off_axi [3];
	ap_uint<_WAxi> 	 dat_ram [3][_BstLen];//local ram depth equals the burst length

	      int        cnt_alltype_fnl;
	      bool       is_onetype_fnl[3];
#pragma HLS RESOURCE variable=dat_ram   core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION    variable=len       complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=offset    complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=off_ali   complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=len_vec   complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=pos_vec   complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=len_ram   complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=off_axi   complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=dat_ram   complete  dim=1
#pragma HLS ARRAY_PARTITION    variable=is_onetype_fnl   complete  dim=1
	/////////////// init ///////////////
	INIT0:
	for(int t=0; t<numType; t++){
#pragma HLS LOOP_TRIPCOUNT min=1 max=1
#pragma HLS PIPELINE II=1
		off_ali[t] =(offset[t])&(scal_char-1);//scal_char is always 2^N
	}
	INIT1:
	for(int t=0; t<numType; t++){
#pragma HLS LOOP_TRIPCOUNT min=1 max=1
#pragma HLS PIPELINE II=1
		len_vec[t] =(len[t]+off_ali[t]+scal_char-1)/scal_char;
		off_axi[t] =(offset[t]+scal_char-1)/scal_char-((off_ali[t])?1:0);
		pos_vec[t] =0;
		len_ram[t] =0;
		pos_ram[t] =0;
		if(t<numType)
			is_onetype_fnl[t] =(len[t]==0);
		else
			is_onetype_fnl[t] = true;
	}

	/////////////// round robin to write ///////////////
	ROUND_ROBIN:
	do{
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=1 max=1
		cnt_alltype_fnl=0;

		is_onetype_fnl[0]=details::axi_batchdata_to_stream<_WAxi, _BstLen, _TStrm0>(
		   rbuf,
		   off_axi [0],
		   len_vec [0],
		   pos_vec [0],
		   dat_ram [0],
		   len_ram [0],
		   pos_ram [0],
		   is_onetype_fnl[0],
		   vec_strm0
		   );
		cnt_alltype_fnl+=(is_onetype_fnl[0]==true);

		is_onetype_fnl[1]=details::axi_batchdata_to_stream<_WAxi, _BstLen, _TStrm1>(
		   rbuf,
		   off_axi [1],
		   len_vec [1],
		   pos_vec [1],
		   dat_ram [1],
		   len_ram [1],
		   pos_ram [1],
		   is_onetype_fnl[1],
		   vec_strm1);
		cnt_alltype_fnl+=(is_onetype_fnl[1]==true);

		is_onetype_fnl[2]=details::axi_batchdata_to_stream<_WAxi, _BstLen, _TStrm2>(
		   rbuf,
		   off_axi [2],
		   len_vec [2],
		   pos_vec [2],
		   dat_ram [2],
		   len_ram [2],
		   pos_ram [2],
		   is_onetype_fnl[2],
		   vec_strm2);
		cnt_alltype_fnl+=(is_onetype_fnl[2]==true);



	}while( cnt_alltype_fnl != numType);
#ifndef __SYNTHESIS__
	printf("cnt_test=%d\n",cnt_test);
#endif
}


template <int _WAxi, typename _TStrm, int scal_vec >
void split_vec_to_aligned_duplicate(
    hls::stream<ap_uint<_WAxi> >& vec_strm,
    const int len,
    const int scal_char,
    const int offset,
    hls::stream<_TStrm >& r_strm,
    hls::stream<bool>& e_strm
){

    const int WStrm = 8*sizeof(_TStrm);
    const int nwrite = (len + sizeof(_TStrm) - 1) / sizeof(_TStrm);
    ap_uint<_WAxi> vec_reg = vec_strm.read();
    ap_uint<_WAxi> vec_aligned = 0;

    if( offset ){

        LOOP_SPLIT_VEC_TO_ALIGNED:
          for (int i = 0; i < nwrite; i += scal_vec) {
        #pragma HLS loop_tripcount min=1 max=1
        #pragma HLS PIPELINE II = scal_vec
              vec_aligned((scal_char-offset<<3)-1, 0)              = vec_reg((scal_char<<3)-1, offset<<3);
        	  if((scal_char-offset)<len){//always need read again
                  ap_uint<_WAxi> vec = vec_strm.read();
                  vec_aligned((scal_char<<3)-1, (scal_char-offset)<<3) = vec(offset<<3, 0);
                  vec_reg    ((scal_char<<3)-1, offset<<3)             = vec((scal_char<<3)-1, offset<<3);
        	  }//else few cases no read again
              int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;
              for (int j = 0; j < scal_vec; ++j) {
        #pragma HLS PIPELINE II = 1
                  ap_uint<WStrm > r0 =
                      vec_aligned.range(WStrm * (j + 1) - 1, WStrm*j);
                  if (j < n) {
                      r_strm.write((_TStrm)r0);
                      e_strm.write(false);
                  }//end if
              }
          }

    }

    if( !offset ){
    	//no read
    	SPLIT_VEC:
		int fst_n =  scal_vec > nwrite ? nwrite  : scal_vec;
		for (int j = 0; j < scal_vec; ++j) {
		#pragma HLS PIPELINE II = 1
			ap_uint<WStrm > r0 =
			vec_reg.range(WStrm * (j + 1) - 1, WStrm * j);
			if (j < fst_n) {
				r_strm.write((_TStrm)r0);
				e_strm.write(false);
			}
		}

		for (int i = scal_vec; i < nwrite; i += scal_vec) {
		#pragma HLS loop_tripcount min=1 max=1
		#pragma HLS PIPELINE II = scal_vec
			ap_uint<_WAxi> vec = vec_strm.read();
			int n = (i + scal_vec) > nwrite ? (nwrite - i) : scal_vec;

//			if(i==599){
//				printf("test");
//			}
			for (int j = 0; j < scal_vec; ++j) {
		#pragma HLS PIPELINE II = 1
				ap_uint<WStrm > r0 =
				vec.range(WStrm * (j + 1) - 1, WStrm * j);
				if (j < n) {
					r_strm.write((_TStrm)r0);
					e_strm.write(false);
				}
			}
		 }
    }
    e_strm.write(true);
}
}// details

// ---------------------- APIs ---------------------------------

/* @brief Loading multiple categories of data from one AXI master to streams.
 *
 * This primitive assumes the width of AXI port is multiple of alignment width.
 * When alignment width is less than AXI port width, the AXI port bandwidth
 * will not be fully used.
 *
 * AXI port width and width of each type are assumed to be multiple of 8.
 * It is assumed that the data width in bits is ``8 * sizeof(T)``, and data
 * type can be casted from raw bits of matching width.
 *
 * This module assumes the data is tightly packed, so that the begining of
 * Type 2 data may be placed in one AXI port row with the end of Type 1 data.
 *
 * \rst
 * ::
 *
 *     AXI word [ elements of Type 1 ........................................ ]
 *     AXI word [ elements of Type 1 ..... end | begin elements of Type 2 ... ]
 *     AXI word [ elements of Type 2 ........................................ ]
 *
 * \endrst
 *
 * @tparam _WAxi width of AXI port, must be power of 2 and between 8 to 512.
 * @tparam _TStrm stream's type, e.g. ap_uint<fixed_width> for a fixed_width stream.
 * @tparam _TStrm data's type.
 *
 * @param rbuf input AXI port.
 * @param ostrm1 output stream of type 0.
 * @param e_ostrm1 end flag for output stream of type 0.
 * @param ostrm2 output stream of type 1.
 * @param e_ostrm2 end flag for output stream of type 1.
 * @param ostrm3 output stream of type 2.
 * @param e_ostrm3 end flag for output stream of type 2.
 * @param num number of data to load from AXI port for each type.
 * @param offset offset for each type, in number of char.
 */
template <int _WAxi, int _BstLen, typename _TStrm0, typename _TStrm1, typename _TStrm2>
void axi_to_multi_stream(
    ap_uint<_WAxi>* rbuf,
    hls::stream<_TStrm0 >& ostrm0,
    hls::stream<bool>& e_ostrm0,
    hls::stream<_TStrm1 >& ostrm1,
    hls::stream<bool>& e_ostrm1,
    hls::stream<_TStrm2 >& ostrm2,
    hls::stream<bool>& e_ostrm2,
	const int len[3],
    const int offset[3]
){
#pragma HLS DATAFLOW
    hls::stream<ap_uint<_WAxi> > vec_strm0;
    hls::stream<ap_uint<_WAxi> > vec_strm1;
    hls::stream<ap_uint<_WAxi> > vec_strm2;
    int off_ali[3];
	const int 	   scal_char = _WAxi/8;
	const int      scal_vec0 = _WAxi/(8*sizeof(_TStrm0));
	const int      scal_vec1 = _WAxi/(8*sizeof(_TStrm1));
	const int      scal_vec2 = _WAxi/(8*sizeof(_TStrm2));
#pragma HLS ARRAY_PARTITION    variable=off_ali complete
#pragma HLS ARRAY_PARTITION    variable=len     complete
#pragma HLS ARRAY_PARTITION    variable=offset  complete
#pragma HLS RESOURCE variable= vec_strm0   core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= vec_strm0   depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= vec_strm1   core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= vec_strm1   depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= vec_strm2   core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= vec_strm2   depth = NONBLOCK_DEPTH
	for(int t=0; t<3; t++){
#pragma HLS LOOP_TRIPCOUNT min=1 max=1
#pragma HLS PIPELINE II=1
		off_ali[t] =(offset[t])&(scal_char-1);//scal_char is always 2^N
	}
	details::read_to_vec_stream<_WAxi,_BstLen,_TStrm0,_TStrm1,_TStrm2>(rbuf, vec_strm0 , vec_strm1 , vec_strm2, len, offset);
	details::split_vec_to_aligned_duplicate<_WAxi, _TStrm0 , scal_vec0>(
			  vec_strm0, len[0], scal_char, off_ali[0],
			  ostrm0, e_ostrm0);
	details::split_vec_to_aligned_duplicate<_WAxi, _TStrm1 , scal_vec1>(
			  vec_strm1, len[1], scal_char, off_ali[1],
			  ostrm1, e_ostrm1);
	details::split_vec_to_aligned_duplicate<_WAxi, _TStrm2 , scal_vec2>(
			  vec_strm2, len[2], scal_char, off_ali[2],
			  ostrm2, e_ostrm2);
}



// TODO for 2 and for 4

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_MULTI_STRM_H

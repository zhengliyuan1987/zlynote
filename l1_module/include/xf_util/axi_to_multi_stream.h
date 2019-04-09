#ifndef XF_UTIL_AXI_TO_MULTI_STRM_H
#define XF_UTIL_AXI_TO_MULTI_STRM_H

#include "xf_util/types.h"
#include "xf_util/enums.h"

// Forward decl
#ifndef __SYNTHESIS__
int cnt_test=0;
#endif

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
    	ap_uint<_WAxi > 		   ram = 0;

    	for(int n=0; n<_BurstLen; n++){
#pragma HLS loop_tripcount min=1 max=1
#pragma HLS PIPELINE II=1
    		dat_ram[n]= vec_ptr[n];
    	}
        if( _BurstLen >= len -pos  ){//All data has been copied
            len_ram = len - pos;
            pos  = len;
        }else {//Full size is copied but still remain one or more
            len_ram = _BurstLen;
            pos +=_BurstLen;
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
		hls::stream<_TStrm >&   ostrm,
		hls::stream<bool>& 		e_ostrm
        )
{
    ap_uint<_WAxi> tmp;
//#pragma HLS data_pack variable=dat_ram
//#pragma HLS data_pack variable=tmp
    if(is_onetype_fnl){
    	e_ostrm.write(true);
        return is_onetype_fnl;
    }
    bool isFinalBlock     = len==pos;
    bool hasDone_blk      = len_ram==pos_ram;
    bool isFull           = false;
    //reorg and split
    ap_uint<_WAxi> vec_reg;

//    if(!pos&&(len>1)){
//    	vec_reg = dat_ram[0];
//    	pos_ram++;
//    }
    NON_BLOCKING_LOOP:
    while( isFull==false && hasDone_blk==false ){
#pragma HLS loop_tripcount min=1 max=32  avg=32
#pragma HLS PIPELINE II=1
    	_TStrm      rd = dat_ram[pos_ram];
        is_onetype_fnl = (isFinalBlock)& (((pos_ram+1)==len_ram))?true:false;
        hasDone_blk    = (((pos_ram+1)==len_ram))?true:false;
        (pos_ram)++;
        isFull         = ostrm.full();
        if(!isFull){
        	ostrm.write(rd);
        	e_ostrm.write(false);
#ifndef __SYNTHESIS__
            cnt_test++;
#endif
        }

    }//end while
   // if(is_onetype_fnl)
   // e_ostrm.write(true);
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
        hls::stream<_TStrm >&	ostrm,
		hls::stream<bool>& 		e_ostrm
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
		ostrm,
        e_ostrm );
	return is_fnl;

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
	const int        numType = 3;
	const int 	   scal_char = _WAxi/8;
	  	  int        len_vec [3];//length for one type vector, by width of Axi
	 	  int        pos_vec [3];//current position for one type vec, by width of Axi
	 	  int        len_ram [3];//length for one type vector loaded in local ram
	 	  int        pos_ram [3];//current position need to be read
	 	  int        off_ali [3];//the first row offset aligned to Axi port by char
	 	  int        off_axi [3];
	ap_uint<_WAxi> 	 dat_ram [3][_BstLen];//local ram depth equals the burst length

#pragma HLS RESOURCE variable=dat_ram   core=RAM_2P_BRAM
//#pragma HLS RESOURCE variable=len_vec   core=RAM_1P_LUTRAM
//#pragma HLS RESOURCE variable=pos_vec   core=RAM_1P_LUTRAM
//#pragma HLS RESOURCE variable=len_ram   core=RAM_1P_LUTRAM
//#pragma HLS RESOURCE variable=pos_ram   core=RAM_1P_LUTRAM
//#pragma HLS ARRAY_PARTITION    variable=pos_ram   complete  dim=1

	int              cnt_alltype_fnl;
	bool             is_onetype_fnl[3];

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
		off_axi[t] =(offset[t]+scal_char-1)/scal_char;
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
		   ostrm0,
		   e_ostrm0);
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
		   ostrm1,
		   e_ostrm1);
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
		   ostrm2,
		   e_ostrm2);
		cnt_alltype_fnl+=(is_onetype_fnl[2]==true);



	}while( cnt_alltype_fnl != numType);
#ifndef __SYNTHESIS__
	printf("cnt_test=%d\n",cnt_test);
#endif
}

// TODO for 2 and for 4

} // level1
} // util
} // xf

// Implementation
// TODO

#endif // XF_UTIL_AXI_TO_MULTI_STRM_H

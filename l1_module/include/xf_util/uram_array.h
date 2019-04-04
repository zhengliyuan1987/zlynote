/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file uram_array.h
 * @brief URAM_ARRAY template function implementation.
 *
 * This file is part of XF Common Utils Library.
 */

#ifndef XF_DATABASE_URAM_ARRAY_H
#define XF_DATABASE_URAM_ARRAY_H

#ifndef __cplusplus
#error \
    "xf_util_level1 uram_array interface, and thus requires C++"
#endif

#include <math.h>
#include <ap_int.h>
#include <iostream>

namespace xf{
namespace util{
namespace level1{

/*
 * @brief the structure of compute available uram block numbers
 *
 * @tparam total size of one block
 * @tparam one   size of one array
 *
 */
template<int total,int one>
struct need_num{
	static const int value = (total + one -1)/one;
};

/*
 * @brief URAMs are 72 bit fixed, and currently very large buffers
 * needs logic help to be read/write every cycle.
 *
 * @tparam WData  the width of every element
 * @tparam NData  the depth of array
 * @tparam NCache the number of cache
 *
 * @param _blocks         the memory of accessing
 * @param _num_uram_block available numbers of uram block
 * @param _elem_per_line  if WData<72,elems of per line,such as WData = 16, _elem_per_line is 4
 * @param _num_index      index numbers of one block
 * @param _num_block      piece of block using one time
 *
 */

template<int WData, int NData, int NCache>
class uram_array{
public:
	uram_array():_elem_per_line(72 / WData),_num_index(_elem_per_line*(4<<10)),_num_block((WData + 71)/72){
#ifndef __SYNTHESIS__
		//TODO:malloc the array
		for(int i=0;i<_num_uram_block;i++)
		{
			_blocks[i] = (ap_uint<72>*)malloc(sizeof(ap_uint<72>) * (4 << 10));
		}
#endif
		for(int i=0;i<4;i++)
		{
			_index[i] = -1;
			_state[i] = 0;
		}
	}
	~uram_array()
	{
#ifndef __SYNTHESIS__
		for(int i=0;i<_num_uram_block;i++)
		{
			free(_blocks[i]);
		}
#endif
	}

	void write(int index, const ap_uint<WData>& d);
	ap_uint<WData> read(int index);
private:
	static const int _num_uram_block;
	const int _elem_per_line;
	const int _num_index;
	const int _num_block;
	int _index[NCache];
	ap_uint<WData> _state[NCache];

#ifndef __SYNTHESIS__
	ap_uint<72>* _blocks[need_num <72 * (4 << 10), WData * NData>::value];
#else
	ap_uint<72>  _blocks[need_num <72 * (4 << 10), WData * NData>::value][4<<10];
#endif
};

template<int WData, int NData, int NCache>
const int uram_array<WData, NData, NCache>::_num_uram_block = need_num <72 * (4 << 10), WData * NData>::value;

// to ensure the array can be updated every cycle.
template<int WData, int NData, int NCache>
void uram_array<WData, NData, NCache>::write(int index, const ap_uint<WData>& d)
{
#pragma HLS inline off
	int div_block=0,div_index=0;
	int dec_block=0,dec=0,begin=0;

#pragma HLS RESOURCE   variable=_blocks core=XPM_MEMORY uram//core=RAM_T2P_URAM
#pragma HLS ARRAY_PARTITION variable=_blocks complete dim=1
#pragma HLS ARRAY_PARTITION variable=_index complete dim=1
#pragma HLS ARRAY_PARTITION variable=_state complete dim=1

#ifndef __SYNTHESIS__
	for(int i=0; i < _num_block; i++)
	{
		if(WData <= 72)
		{
			div_block = index / _num_index;          //which block to write
			dec_block = index % _num_index;          //offset of block
			div_index = dec_block / _elem_per_line;  //which 72bit to write
			dec       = dec_block % _elem_per_line;  //offset of inner 72bit
			begin     = dec * WData;                 //the start which you want to write
			_blocks[div_block][div_index].range(begin+WData-1,begin) = d;
		}
		else
		{

			if(i == (_num_block-1) )
			{
				_blocks[i][index] = d.range( WData -1, ((i<<6)+(i<<3)));
			}
			else
				_blocks[i][index] = d.range( ((i<<6)+(i<<3)) + 71, ((i<<6)+(i<<3)));
		}
	}
#else
	Write_Inner:for(int i=0; i < _num_block; i++)
	{
	#pragma HLS UNROLL factor=15
		if(WData <= 72)
		{
			div_block = index / _num_index;
			dec_block = index % _num_index;
			div_index = dec_block / _elem_per_line;
			dec       = dec_block % _elem_per_line;
			begin     = dec * WData;
			_blocks[div_block][div_index].range(begin+WData-1,begin) = d;
		}
		else
		{
			if(i == (_num_block -1 ))
				_blocks[i][index] = d.range( WData -1, ((i<<6)+(i<<3)));
			else
				_blocks[i][index] = d.range( ((i<<6)+(i<<3)) + 71, ((i<<6)+(i<<3)));
		}
	}
#endif

	Write_Cache:for(int i=NCache-1;i>=1;i--)
	{
#pragma HLS UNROLL factor=3
#pragma HLS DEPENDENCE variable=_state inter false
#pragma HLS DEPENDENCE variable=_state intra false
#pragma HLS DEPENDENCE variable=_index inter false
#pragma HLS DEPENDENCE variable=_index intra false
		_state[i] = _state[i-1];
		_index[i] = _index[i-1];
	}
	_state[0] = d;
	_index[0] = index;
}

template<int WData, int NData,int NCache>
ap_uint<WData> uram_array<WData, NData, NCache>::read(int index)
{
#pragma HLS inline off
	ap_uint<WData> value;
	int div_block=0,div_index=0;
	int dec_block=0,dec=0,begin=0;
	bool is_InState = false;

#pragma HLS RESOURCE variable=_blocks core=XPM_MEMORY uram//core=RAM_T2P_URAM
#pragma HLS ARRAY_PARTITION variable=_blocks complete dim=1
#pragma HLS ARRAY_PARTITION variable=_index  complete dim=1
#pragma HLS ARRAY_PARTITION variable=_state  complete dim=1

	Read_Cache:for(int i=0;i<NCache;i++)
	{
#pragma HLS UNROLL factor=4
#pragma HLS DEPENDENCE variable=_state inter false
#pragma HLS DEPENDENCE variable=_state intra false
#pragma HLS DEPENDENCE variable=_index inter false
#pragma HLS DEPENDENCE variable=_index intra false
		if(index == _index[i])
		{
			value = _state[i];
			is_InState = true;
		}
	}
	if(!is_InState)
	{
#ifndef __SYNTHESIS__
		for(int i=0; i < _num_block; i++)
		{
			if(WData <= 72)
			{
				div_block = index / _num_index;
				dec_block = index % _num_index;
				div_index = dec_block / _elem_per_line;
				dec       = dec_block % _elem_per_line;
				begin     = dec * WData;
				value     = _blocks[div_block][div_index].range(begin+WData-1,begin);
			}
			else
			{

				if(i == (_num_block -1 ))
					value.range( WData -1, ((i<<6)+(i<<3)) ) = _blocks[i][index];
				else
					value.range(((i<<6)+(i<<3)) + 71, ((i<<6)+(i<<3))) = _blocks[i][index];
			}
		}
#else
		Read_Inner:for(int i=0; i < _num_block; i++)
		{
#pragma HLS UNROLL factor=15
			if(WData <= 72)
			{
				div_block = index / _num_index;
				dec_block = index % _num_index;
				div_index = dec_block / _elem_per_line;
				dec       = dec_block % _elem_per_line;
				begin     = dec * WData;
				value     = _blocks[div_block][div_index].range(begin+WData-1,begin);
			}
			else
			{
				if(i == (_num_block -1 ))
					value.range( WData -1, ((i<<6)+(i<<3))) = _blocks[i][index];
				else
					value.range( ((i<<6)+(i<<3)) + 71, ((i<<6)+(i<<3))) = _blocks[i][index];
			}
		}
#endif
	}

	return value;
}

} //level1
} //dal
} //xf

#endif

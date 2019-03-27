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
 * @file direct_aggregate.h
 * @brief DIRECT AGGREGATE template function implementation.
 *
 * This file is part of XF Database Library.
 */

#ifndef XF_DATABASE_URAM_ARRAY_H
#define XF_DATABASE_URAM_ARRAY_H

#ifndef __cplusplus
#error \
    "xf_database_direct_aggregate hls::stream<> interface, and thus requires C++"
#endif

#include <ap_int.h>
#include <iostream>

namespace xf{
namespace dal{

template<int total,int one>
struct need_num{
	static const int value = (total + one -1)/one;
};

template<int WData, int NData>
class uram_array{
public:
	uram_array(){
#ifndef __SYNTHESIS__
	//TODO:malloc the array
	for(int i=0;i<_num_uram_block;i++)
	{
		_blocks[i] = (ap_uint<72>*)malloc(sizeof(ap_uint<72>) * (4 << 10));
	}

#endif
	}
	~uram_array();

	void write(int index, const ap_uint<WData>& d);
	ap_uint<WData> read(int index);
private:
	static const int _num_uram_block;

	static int index_r[3];

	static ap_uint<WData> state_r[3];

#ifndef __SYNTHESIS__
	ap_uint<72>* _blocks[need_num <72 * (4 << 10), WData * NData>::value];
#else
	ap_uint<72>  _blocks[need_num <72 * (4 << 10), WData * NData>::value][4<<10];
#endif
};

template<int WData, int NData>
const int uram_array<WData, NData>::_num_uram_block = need_num <72 * (4 << 10), WData * NData>::value;

template<int WData, int NData>
int uram_array<WData, NData>::index_r[3]={-1,-1,-1};

template<int WData, int NData>
ap_uint<WData> uram_array<WData, NData>::state_r[3]={0,0,0};

// to ensure the array can be updated every cycle.
template<int WData, int NData>
void uram_array<WData, NData>::write(int index, const ap_uint<WData>& d)
{
	int num_index;//one block numbers of index
	int div, dec;
	ap_uint<WData> value;

	if(WData <= 64)
	{
		num_index = (4 << 10) * (72 / WData);
		div = index / ((num_index * _num_uram_block) -1);
		dec = index % ((num_index * _num_uram_block) -1);
	}
	else
	{
		num_index = (4 << 10) / ((WData / 72) + 1);
		div = index / ((num_index * _num_uram_block) -1);
		dec = index % ((num_index * _num_uram_block) -1);
	}

	if(div < _num_uram_block)
	{
#ifndef __SYNTHESIS__

		ap_uint<72>* pDiv = _blocks[div];
		pDiv += dec*(WData / 64);
		if(WData <= 64)
		{
			*pDiv = d;
			value = *pDiv;
		}
		else
		{
			for(int i=0;i<WData/64;i++)
			{
#pragma HLS PIPELINE II=1
				*pDiv = d( i*64 + 63, i*63);
				value(i*64 + 63, i*63) = *pDiv;
				pDiv += 1;
			}

		}
	std::cout<< "write local_value="<< value << std::endl;
#else
		dec = dec*(WData / 64);
		if(WData <= 64)
		{
			_blocks[div][dec] = d;
			value = _blocks[div][dec];
		}
		else
		{
			for(int i=0;i<WData/64;i++)
			{
#pragma HLS PIPELINE II=1
				_blocks[div][dec+i] = d( i*64 + 63, i*63);
				//value(i*64 + 63, i*63) = _blocks[div][dec+i];
			}
		}
#endif
	}
}

template<int WData, int NData>
ap_uint<WData> uram_array<WData, NData>::read(int index)
{
	ap_uint<WData> value;

	int num_index;//one block numbers of index
	int div, dec;

	if(index == index_r[0])
		value = state_r[0];
	else if(index == index_r[1])
		value = state_r[1];
	else if(index == index_r[2])
		value = state_r[2];
	else
	{

		if(WData <= 64)
		{
			num_index = (4 << 10) * (72 / WData);
			div = index / ((num_index * _num_uram_block) -1);
			dec = index % ((num_index * _num_uram_block) -1);
		}
		else
		{
			num_index = (4 << 10) / ((WData / 72) + 1);
			div = index / ((num_index * _num_uram_block) -1);
			dec = index % ((num_index * _num_uram_block) -1);
		}

		if(div < _num_uram_block)
		{
	#ifndef __SYNTHESIS__
			ap_uint<72>* pDiv = _blocks[div];
			pDiv += dec*(WData / 64);

			if(WData <= 64)
			{
				value = *pDiv;
			}
			else
			{
				for(int i=0;i<WData/64;i++)
				{
					value(i*64 + 63, i*63) = *pDiv;
					pDiv += 1;
				}

			}
	#else
			dec = dec*(WData / 64);
			if(WData <= 64)
			{
				value = _blocks[div][dec];
			}
			else
			{
				for(int i=0;i<WData/64;i++)
				{
#pragma HLS PIPELINE II=1
					value(i*64 + 63, i*63) = _blocks[div][dec+i];
				}
			}
	#endif
		}
	}
	state_r[2] = state_r[1];
	state_r[1] = state_r[0];
	state_r[0] = value;
	index_r[2] = index_r[1];
	index_r[1] = index_r[0];
	index_r[0] = index;

	return value;
}

template<int WData, int NData>
uram_array<WData, NData>::~uram_array()
{
#ifndef __SYNTHESIS__
	for(int i=0;i<_num_uram_block;i++)
	{
		free(_blocks[i]);
	}
#endif
}

} //dal
} //xf

#endif

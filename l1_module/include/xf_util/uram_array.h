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
    "xf_util_level1 uram_array interface, and thus requires C++"
#endif

#include <math.h>
#include <ap_int.h>
#include <iostream>

namespace xf{
namespace util{
namespace level1{

#define   N   5

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
		for(int i=0;i<N;i++)
		{
			index_r[i] = -1;
			state_r[i] = 0;
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

	bool write(int index, const ap_uint<WData>& d);
	ap_uint<WData> read(int index);
private:
	static const int _num_uram_block;

	int index_r[N];

	ap_uint<WData> state_r[N];

#ifndef __SYNTHESIS__
	ap_uint<72>* _blocks[need_num <72 * (4 << 10), WData * NData>::value];
#else
	ap_uint<72>  _blocks[need_num <72 * (4 << 10), WData * NData>::value][4<<10];
#endif
};

template<int WData, int NData>
const int uram_array<WData, NData>::_num_uram_block = need_num <72 * (4 << 10), WData * NData>::value;

// to ensure the array can be updated every cycle.
template<int WData, int NData>
bool uram_array<WData, NData>::write(int index, const ap_uint<WData>& d)
{
	int num_block;//block numbers of need
	int WData_padding;
	ap_uint<WData> value;
#pragma HLS RESOURCE variable=_blocks core=XPM_MEMORY uram//core=RAM_T2P_URAM

//	if((WData & (WData-1)) != 0)//no 2^n
//	{
//		WData_padding = WData;
//		WData_padding |= WData_padding >> 1;
//		WData_padding |= WData_padding >> 2;
//		WData_padding |= WData_padding >> 4;
//		WData_padding |= WData_padding >> 8;
//		WData_padding |= WData_padding >> 16;
//		WData_padding  = WData_padding +  1;
//	}
//	else
//	{
//		WData_padding = WData;
//	}

	num_block = (WData + 71) / 72;

	if(num_block <= _num_uram_block)
	{
#ifndef __SYNTHESIS__
		ap_uint<72> *pDiv;

		if(num_block <= 1)
		{
			pDiv = _blocks[0] + index;
			*pDiv = d;
			//value = *pDiv;
		}
		else
		{
			for(int i=0; i < num_block; i++)
			{
				pDiv = _blocks[i]+index;
				if(i == (num_block-1) )
				{
					*pDiv = d( WData -1, i*72);
				}
				else
					*pDiv = d( i*72 + 71, i*72);
			}
		}
		//std::cout<< "write local_value="<< value << std::endl;
#else
		if(num_block <= 1)
		{
			_blocks[0][index] = d;
		}
		else
		{
			for(int i=0; i < num_block; i++)
			{
#pragma HLS PIPELINE II=1
				if(i == (num_block -1 ))
				{
					_blocks[i][index] = d( WData -1, i*72);
				}
				else
					_blocks[i][index] = d( i*72 + 71, i*72);
			}
		}
#endif

		for(int i=N-1;i>=1;i--)
		{
			state_r[i] = state_r[i-1];
			index_r[i] = index_r[i-1];
		}
		state_r[0] = d;
		index_r[0] = index;
	}
	else
	{
		std::cout<< "The WData="<< WData<< " NData="<< NData << " is not expansion."<< std::endl;
		return false;
	}
	return true;
}

template<int WData, int NData>
ap_uint<WData> uram_array<WData, NData>::read(int index)
{
	ap_uint<WData> value;
	bool is_InState = false;

	int num_block;//one block numbers of index
	int WData_padding;
#pragma HLS RESOURCE variable=_blocks core=XPM_MEMORY uram//core=RAM_T2P_URAM

	for(int i=0;i<N;i++)
	{
		if(index == index_r[i])
		{
			value = state_r[i];
			is_InState = true;
			break;
		}
	}
	if(!is_InState)
	{
//		if((WData & (WData-1)) != 0)//no 2^n
//		{
//			WData_padding = WData;
//			WData_padding |= WData_padding >> 1;
//			WData_padding |= WData_padding >> 2;
//			WData_padding |= WData_padding >> 4;
//			WData_padding |= WData_padding >> 8;
//			WData_padding |= WData_padding >> 16;
//			WData_padding = WData_padding + 1;
//		}

		num_block = (WData + 71) / 72;

		if(num_block <= _num_uram_block)
		{
#ifndef __SYNTHESIS__
			ap_uint<72> *pDiv;
			if(num_block <= 1)
			{
				pDiv = _blocks[0] + index;
				value = *pDiv;
			}
			else
			{
				for(int i=0; i < num_block; i++)
				{
					pDiv = _blocks[i]+index;
					if(i == (num_block -1 ))
					{
						value( WData -1, i*72) = *pDiv;
					}
					else
						value(i*72 + 71, i*72) = *pDiv;
				}
			}
#else
			if(num_block <= 1)
			{
				value = _blocks[0][index];
			}
			else
			{
				for(int i=0; i < num_block; i++)
				{
#pragma HLS PIPELINE II=1
					if(i == (num_block -1 ))
					{
						value( WData -1, i*72) = _blocks[i][index];
					}
					else
						value( i*72 + 71, i*72) = _blocks[i][index];
				}
			}
#endif
		}
		else
			std::cerr<< "The WData="<< WData<< " NData="<< NData << " is not expansion."<< std::endl;
	}

	return value;
}

} //level1
} //dal
} //xf

#endif

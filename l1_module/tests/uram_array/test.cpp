
#include <iostream>
#include <stdlib.h>
#include <ap_int.h>

#include "hls_stream.h"
#include "xf_util/uram_array.h"

#define NUM_SIZE     2048
#define WDATA        64
#define NDATA        1024

void uram_array_test()
{
	ap_uint<WDATA> data1,data2;
	xf::util::level1::uram_array<WDATA,NDATA> uram_array1;

	WRITE_URAM:for(int i=0;i<NUM_SIZE;i++)
	{
#pragma HLS PIPELINE

		if(!uram_array1.write(i, 1))
			break;
	}

	std::cout<<std::left<<std::fixed;
	READ_URAN:for(int i=0;i<NUM_SIZE;i++)
	{
#pragma HLS PIPELINE
		data1 = uram_array1.read(i);
		data2 = uram_array1.read(i+1);
		data2 = data1 + data2;
		uram_array1.write(i+1, data2);
		data1 = uram_array1.read(i);
		std::cout<< "index="<<i<< "  data="<< data1 << std::endl;
	}
}

int main()
{
	uram_array_test();
	return 0;
}

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <ap_int.h>

#include "hls_stream.h"
#include "xf_util/uram_array.h"

#define INT_RAND_MAX 1024
#define WDATA        128
#define NDATA        1024


void uram_array_test()
{
	ap_uint<WDATA> data;
	xf::dal::uram_array<WDATA,NDATA> uram_array1;

	for(int i=0;i<NDATA;i++)
	{
		uram_array1.write(i, i+1);
	}

	data = uram_array1.read(1023);
	std::cout<< "read data="<< data << std::endl;

}

int main()
{
	uram_array_test();
	return 0;
}

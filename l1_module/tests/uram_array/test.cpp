
#include <iostream>
#include <stdlib.h>
#include <ap_int.h>

#include "hls_stream.h"
#include "xf_util/uram_array.h"

//as reference uram size 4K*256
// need to NUM_SIZE <= NDATA
#define NUM_SIZE   (10<<10)
#define WDATA      64
#define NDATA      (20<<10)
#define NCACHE     4

void update_logic(ap_uint<WDATA>& out)
{
#pragma HLS inline
	out += out;
	//out++;
}

void core_test(hls::stream<ap_uint<WDATA> >& out_stream)
{
	ap_uint<WDATA> out;
	xf::util::level1::uram_array<WDATA,NDATA,NCACHE> uram_array1;

	LOOP: for(int i=0;i<NUM_SIZE;i++)
	{
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=uram_array1._blocks intra false
#pragma HLS DEPENDENCE variable=uram_array1._blocks inter false

			uram_array1.write(i, i);
			out = uram_array1.read(i);
			//add your logic
			update_logic(out);
			uram_array1.write(i, out);
			out_stream.write(out);
	}
}

int uram_array_test()
{
	int nerror = 0;
	ap_uint<WDATA> in, out, tmp;
	hls::stream<ap_uint<WDATA> > out_stream("ref_output");

	xf::util::level1::uram_array<WDATA,NDATA,NCACHE> uram_array2;

	for(int i=0;i<NUM_SIZE;i++)
	{
		tmp=i;
		update_logic(tmp);
		uram_array2.write(i, tmp);
	}

	core_test(out_stream);

	for(int i=0;i<NUM_SIZE;i++)
	{
		in = uram_array2.read(i);
		out = out_stream.read();
		bool cmp = (in == out) ? 1 : 0;
		if(!cmp)
		{
			nerror++;
			std::cout<<"The data is incorrect." << std::endl;
		}
	}

	if (nerror)
	{
		std::cout << "\nFAIL: " << nerror << "the result is wrong.\n";
	}
	else
	{
		std::cout << "\nPASS: no error found.\n";
	}
	return nerror;
}

int main()
{
	int inteval = 0;

	inteval = uram_array_test();

	return inteval;
}

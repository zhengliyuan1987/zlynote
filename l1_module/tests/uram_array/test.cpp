
#include <iostream>
#include <stdlib.h>
#include <ap_int.h>

#include "hls_stream.h"
#include "xf_util/uram_array.h"


/******************** range as reference *******************
 * WData	1	    16	   64	 128   256	512	 1024
 * NData	294912	16384  4096	 2048  350	80	 20
 * block	1	    1	   1	 2	   4	8	 15
 * *********************************************************/

#define NUM_SIZE     1024
#define WDATA        128
#define NDATA        2048
#define NCACHE       4

void core_test(hls::stream<ap_uint<WDATA> >& ins, hls::stream<ap_uint<WDATA> >& outs)
{
	ap_uint<WDATA> in,out;
	xf::util::level1::uram_array<WDATA,NDATA,NCACHE> uram_array1;

	W_LOOP: for(int i=0;i<NUM_SIZE;i++)
	{
//#pragma HLS PIPELINE II=1
		in = ins.read();
		uram_array1.write(i, in);
	}

	R_LOOP:for(int i=0;i<NUM_SIZE;i++)
	{
//#pragma HLS PIPELINE II=1
		out = uram_array1.read(i);
		outs.write(out);
	}
}

int uram_array_test()
{
	hls::stream<ap_uint<WDATA> > ins, outs;
	int in,out;
	int nerror = 0;

	for(int i=0;i<NUM_SIZE;i++)
	{
		ins.write(i+1);
	}
	core_test(ins,outs);

	for(int i=0;i<NUM_SIZE;i++)
	{
		if(!outs.empty())
			out = outs.read();

		if(out != (i+1) )
		{
			nerror = 1;
		}

		std::cout<<out<<std::endl;
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

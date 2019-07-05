#ifndef _DUT_H_
#define _DUT_H_
#include "ap_int.h"
#include "hls_stream.h"
#include "xf_utils_hw/uram_array.hpp"

// as reference uram size 4K*256
#define WDATA (64)
#define NDATA (16 << 10)
#define NCACHE (4)

#define NUM_SIZE (1 << 10)

void dut(ap_uint<WDATA> ii, hls::stream<ap_uint<WDATA> > &out_stream);

#endif // _DUT_H_

#include "dut.hpp"

void dut(ap_uint<WDATA> ii, hls::stream<ap_uint<WDATA> > &out_stream) {
  xf::common::utils_hw::uram_array<WDATA, NDATA, NCACHE> uram_array1;

l_init_value:
  int num = uram_array1.memset(ii);

l_read_after_write_test:
  for (int i = 0; i < NUM_SIZE; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = uram_array1.blocks inter false
    if ((i & 1) == 0) {
      uram_array1.write(i, i);
    } else {
      ap_uint<WDATA> t = uram_array1.read(i - 1);
      out_stream.write(t);
    }
  }

// test case requires WData > 36, otherwise cosim will fail
l_update_value_with_1_II:
  for (int i = 0; i < NUM_SIZE; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = uram_array1.blocks inter false
    ap_uint<WDATA> t = uram_array1.read(i);
    ap_uint<WDATA> u = (t & 1) ? 1 : 0;
    uram_array1.write(i, u);
  }

l_dump_value:
  for (int i = 0; i < NDATA; ++i) {
#pragma HLS PIPELINE II = 1
    ap_uint<WDATA> t = uram_array1.read(i);
    out_stream.write(t);
  }
}


#include <ap_int.h>
#include <stdlib.h>
#include <iostream>

#include "hls_stream.h"
#include "xf_util/uram_array.h"

// as reference uram size 4K*256
#define WDATA (64)
#define NDATA (4 << 10)
#define NCACHE (4)

#define NUM_SIZE (4 << 10)

void core_test(hls::stream<ap_uint<WDATA> >& out_stream) {
  xf::util::level1::uram_array<WDATA, NDATA, NCACHE> uram_array1;

l_init_value:
  for (int i = 0; i < NDATA; ++i) {
#pragma HLS PIPELINE II = 1
    uram_array1.write(i, 0);
  }

l_read_after_write_test:
  for (int i = 0; i < NUM_SIZE; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = uram_array1._blocks inter false
    if ((i & 1) == 0) {
      uram_array1.write(i, i);
    } else {
      uram_array1.write(i, i);
      ap_uint<WDATA> t = uram_array1.read(i - 1);
      out_stream.write(t);
    }
  }

l_update_value_with_1_II:
  for (int i = 0; i < NUM_SIZE; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = uram_array1._blocks intra false
    ap_uint<WDATA> t = uram_array1.read(i);
    ap_uint<WDATA> u = (t & 1) ? 1 : 0;
    uram_array1.write(i, u + i);
  }

l_dump_value:
  for (int i = 0; i < NDATA; ++i) {
#pragma HLS PIPELINE II = 1
    ap_uint<WDATA> t = uram_array1.read(i);
    out_stream.write(t);
  }
}

int uram_array_test() {
  int nerror = 0;

  hls::stream<ap_uint<WDATA> > ref_stream("reference");
  ap_uint<WDATA> ref_array[NDATA];
  for (int i = 0; i < NDATA; ++i) {
    ref_array[i] = 0;
  }
  for (int i = 0; i < NUM_SIZE; ++i) {
    if ((i & 1) == 0) {
      ref_array[i] = i;
    } else {
      ref_array[i] = i;
      ap_uint<WDATA> t = ref_array[i - 1];
      ref_stream.write(t);
    }
  }
  for (int i = 0; i < NUM_SIZE; i++) {
    ap_uint<WDATA> t = ref_array[i];
    ap_uint<WDATA> u = (t & 1) ? 1 : 0;
    ref_array[i] = u + i;
  }
  for (int i = 0; i < NDATA; ++i) {
    ap_uint<WDATA> t = ref_array[i];
    ref_stream.write(t);
  }

  hls::stream<ap_uint<WDATA> > out_stream("output");
  core_test(out_stream);

  while (true) {
    ap_uint<WDATA> r = ref_stream.read();
    ap_uint<WDATA> o = out_stream.read();
    if (r != o) {
      if (!nerror)
        std::cout << "The data is incorrect, check implementation."
                  << std::endl;
      nerror++;
    }
    if (ref_stream.size() == 0) {
      if (out_stream.size() == 0) {
        break;
      } else {
        std::cout << "The number of data is incorrect, check test case"
                  << std::endl;
      }
    } else if (out_stream.size() == 0) {
      std::cout << "The number of data is incorrect, check test case"
                << std::endl;
    }
  }

  if (nerror) {
    std::cout << "\nFAIL: " << nerror << " elements are wrong.\n";
  } else {
    std::cout << "\nPASS: no error found.\n";
  }
  return nerror;
}

int main() { return uram_array_test(); }


#include <ap_int.h>
#include <stdlib.h>
#include <iostream>

#include "hls_stream.h"
#include "xf_util/uram_array.h"

// as reference uram size 4K*256
// need to NUM_SIZE <= NDATA
#define NUM_SIZE (10 << 10)
#define WDATA 128
#define NDATA (20 << 10)
#define NCACHE 4

void update_logic(ap_uint<WDATA>& out) {
#pragma HLS inline
  out += out;
  // out++;
}

void core_test1(hls::stream<ap_uint<WDATA> >& out_stream) {
  ap_uint<WDATA> out;
  xf::util::level1::uram_array<WDATA, NDATA, NCACHE> uram_array1;

Loop1:
  for (int i = 0; i < NUM_SIZE; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = uram_array1._blocks intra false
#pragma HLS DEPENDENCE variable = uram_array1._blocks inter false
    if ((i & 1) == 0) {
      uram_array1.write(i, i);
    } else {
      uram_array1.write(i, i);
      out = uram_array1.read(i - 1);
      // add your logic
      update_logic(out);
      out_stream.write(out);
    }
  }
}

void core_test2(hls::stream<ap_uint<WDATA> >& out_stream) {
  ap_uint<WDATA> out;
  xf::util::level1::uram_array<WDATA, NDATA, NCACHE> uram_array1;

Loop2:
  for (int i = 0; i < NUM_SIZE; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = uram_array1._blocks intra false
#pragma HLS DEPENDENCE variable = uram_array1._blocks inter false

    uram_array1.write(i, i);
    out = uram_array1.read(i);
    // add your logic
    update_logic(out);
    uram_array1.write(i, out);
    out_stream.write(out);
  }
}

int uram_array_test1() {
  int nerror = 0;
  ap_uint<WDATA> in, out, tmp;
  hls::stream<ap_uint<WDATA> > out_stream("ref_output");

  xf::util::level1::uram_array<WDATA, NDATA, NCACHE> uram_array1;

  for (int i = 0; i < NUM_SIZE; i++) {
    if ((i & 1) == 0) {
      tmp = i;
      update_logic(tmp);
      uram_array1.write(i, tmp);
    }
  }

  core_test1(out_stream);

  for (int i = 0; i < NUM_SIZE / 2; i++) {
    in = uram_array1.read(i * 2);
    out = out_stream.read();
    bool cmp = (in == out) ? 1 : 0;
    if (!cmp) {
      nerror++;
      std::cout << "The data is incorrect." << std::endl;
    }
    // std::cout<<in<<"  " <<out<< std::endl;
  }

  if (nerror) {
    std::cout << "\nFAIL: " << nerror << "the result is wrong.\n";
  } else {
    std::cout << "\nPASS: no error found.\n";
  }
  return nerror;
}

int uram_array_test2() {
  int nerror = 0;
  ap_uint<WDATA> in, out, tmp;
  hls::stream<ap_uint<WDATA> > out_stream("ref_output");

  xf::util::level1::uram_array<WDATA, NDATA, NCACHE> uram_array1;

  for (int i = 0; i < NUM_SIZE; i++) {
    tmp = i;
    update_logic(tmp);
    uram_array1.write(i, tmp);
  }

  core_test2(out_stream);

  for (int i = 0; i < NUM_SIZE; i++) //
  {
    in = uram_array1.read(i);
    out = out_stream.read();
    bool cmp = (in == out) ? 1 : 0;
    if (!cmp) {
      nerror++;
      std::cout << "The data is incorrect." << std::endl;
    }
    // std::cout<<in<<"  " <<out<< std::endl;
  }

  if (nerror) {
    std::cout << "\nFAIL: " << nerror << "the result is wrong.\n";
  } else {
    std::cout << "\nPASS: no error found.\n";
  }
  return nerror;
}

int main() {
  int inteval = 0;

  // inteval = uram_array_test1();
  inteval = uram_array_test2();

  return inteval;
}

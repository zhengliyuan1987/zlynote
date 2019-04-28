
#include <ap_int.h>
#include <iostream>
#include <stdlib.h>

#include "hls_stream.h"
#include "xf_util/stream_to_axi.h"

#define WAXI (512)
#define WSTRM (32)

#define SNUM (512)

// generate a random integer sequence between speified limits a and b (a<b);
int rand_int(int a, int b) { return rand() % (b - a + 1) + a; }
// burst default = 16
void stream_to_axi_test(ap_uint<WAXI> wbuf[SNUM * WSTRM / WAXI + 1],
                        hls::stream<ap_uint<WSTRM> > &istrm,
                        hls::stream<bool> &e_strm) {
#pragma HLS INTERFACE s_axilite port = wbuf bundle = control
#pragma HLS INTERFACE m_axi depth = 512 port = wbuf offset = slave bundle =    \
    gmem0 num_read_outstanding = 32 num_write_outstanding =                    \
        32 max_read_burst_length = 16 max_write_burst_length = 16
  xf::util::level1::stream_to_axi<WAXI, WSTRM>(wbuf, istrm, e_strm);
}

int main() {
  int nerror = 0;

  hls::stream<ap_uint<WSTRM> > in_stream("in_stream");
  hls::stream<bool> e_strm("e_strm");

  // part of divisible

  const int N = WAXI / WSTRM;
  const int NUM = SNUM * WSTRM / WAXI;

  ap_uint<WAXI> buf[NUM + 1];
  ap_uint<WAXI> ref_buf[NUM + 1];
  ap_uint<WAXI> tmp;

  for (int i = 0; i < (NUM + 1); i++) {
    ref_buf[i] = 0;
    buf[i] = 0;
  }

  for (int i = 0; i < SNUM; i++) {
    int bs = i % N;
    int offset = bs * WSTRM;
    ap_uint<WSTRM> w = rand_int(1, 32);
    tmp(offset + WSTRM - 1, offset) = w(WSTRM - 1, 0);
    if (bs == (N - 1)) {
      ref_buf[i / N] = tmp;
    }
    if (bs != (N - 1) && (i == (SNUM - 1))) {
      tmp(WSTRM - 1, (bs + 1) * WSTRM) = 0;
      ref_buf[i / N] = tmp;
    }
    in_stream.write(w);
    e_strm.write(0);
  }
  e_strm.write(1);

  std::cout << std::endl;

  stream_to_axi_test(buf, in_stream, e_strm);

  for (int i = 0; i < (NUM + 1); i++) {
    bool cmp = (buf[i] == ref_buf[i]) ? 1 : 0;
    if (!cmp) {
      nerror++;
    }
  }

  if (nerror) {
    std::cout << "\nFAIL: nerror= " << nerror << " errors found.\n";
  } else {
    std::cout << "\nPASS: no error found.\n";
  }

  return nerror;
}

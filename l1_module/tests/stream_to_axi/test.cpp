
#include <ap_int.h>
#include <stdlib.h>
#include <iostream>

#include "hls_stream.h"
#include "xf_util/stream_to_axi.h"

#define WAxi (512)
#define WStrm (32)

#define SNum (512)
#define NUM (SNum * WStrm / WAxi)

// generate a random integer sequence between speified limits a and b (a<b);
int rand_int(int a, int b) { return rand() % (b - a + 1) + a; }

// default burst = 16
void stream_to_axi_test(ap_uint<WAxi> wbuf[NUM + 1],
                        hls::stream<ap_uint<WStrm> >& istrm,
                        hls::stream<bool>& e_strm) {
  xf::util::level1::stream_to_axi<WAxi, WStrm>(wbuf, istrm, e_strm);
}

int main() {
  int nerror = 0;

  hls::stream<ap_uint<WStrm> > in_stream("in_stream");
  hls::stream<ap_uint<WStrm> > ref_in_stream("ref_in_stream");
  hls::stream<bool> e_strm("e_strm");

  // part of divisible

  const int N = WAxi / WStrm;

  ap_uint<WAxi> buf[NUM + 1];
  ap_uint<WAxi> ref_buf[NUM + 1];
  ap_uint<WAxi> tmp;

  for (int i = 0; i < (NUM + 1); i++) {
    ref_buf[i] = 0;
    buf[i] = 0;
  }

  for (int i = 0; i < SNum; i++) {
    int bs = i % N;
    int offset = bs * WStrm;
    ap_uint<WStrm> w = rand_int(1, 32);
    tmp(offset + WStrm - 1, offset) = w(WStrm - 1, 0);
    if (bs == (N - 1)) {
      ref_buf[i / N] = tmp;
    }
    if (bs != (N - 1) && (i == (SNum - 1))) {
      tmp(WAxi - 1, (bs + 1) * WStrm) = 0;
      ref_buf[i / N] = tmp;
    }
    in_stream.write(w);
    ref_in_stream.write(w);
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

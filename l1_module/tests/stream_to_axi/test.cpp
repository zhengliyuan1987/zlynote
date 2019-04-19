
#include <ap_int.h>
#include <stdlib.h>
#include <iostream>

#include "hls_stream.h"
#include "xf_util/stream_to_axi.h"

#define WAxi (512)
#define WStrm (32)
#define NBurst (16)

#define SNum (512)

// generate a random integer sequence between speified limits a and b (a<b);
int rand_int(int a, int b) { return rand() % (b - a + 1) + a; }

void stream_to_axi_test(ap_uint<WAxi> wbuf[(SNum * WStrm) / WAxi],
                        hls::stream<ap_uint<WStrm> >& istrm,
                        hls::stream<bool>& e_strm) {
  xf::util::level1::stream_to_axi<WAxi, WStrm, NBurst>(wbuf, istrm, e_strm);
}

int main() {
  int nerror = 0;

  hls::stream<ap_uint<WStrm> > in_stream("in_stream");
  hls::stream<ap_uint<WStrm> > ref_in_stream("ref_in_stream");
  hls::stream<bool> e_strm("e_strm");

  const int num = (SNum * WStrm) / WAxi; // divisible
  ap_uint<WAxi> buf[num];
  ap_uint<WAxi> ref_buf[num];
  ap_uint<WAxi> tmp;
  const int N = WAxi / WStrm;

  for (int i = 0; i < SNum; i++) {
    int bs = i % N;
    int offset = bs * WStrm;
    ap_uint<WStrm> w = rand_int(1, 32);
    tmp(offset + WStrm - 1, offset) = w(WStrm - 1, 0);
    if (bs == (N - 1)) {
      ref_buf[i / N] = tmp;
      buf[i / N] = 0;

      // std::cout<< "ref_buf[" <<i/N <<"]="<< ref_buf[i/N] << std::endl;
    }
    in_stream.write(w);
    ref_in_stream.write(w);
    e_strm.write(0);
  }
  e_strm.write(1);

  std::cout << std::endl;

  stream_to_axi_test(buf, in_stream, e_strm);

  for (int i = 0; i < NBurst; i++) {
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
